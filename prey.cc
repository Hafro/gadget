#include "prey.h"
#include "errorhandler.h"
#include "readword.h"
#include "readaggregation.h"
#include "keeper.h"
#include "mathfunc.h"
#include "gadget.h"

extern ErrorHandler handle;

Prey::Prey(CommentStream& infile, const IntVector& Areas, const char* givenname)
  : HasName(givenname), LivesOnAreas(Areas), CI(0) {

  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i;

  //read the prey length group data
  DoubleVector preylengths;
  CharPtrVector preylenindex;
  char aggfilename[MaxStrLength];
  strncpy(aggfilename, "", MaxStrLength);
  ifstream datafile;
  CommentStream subdata(datafile);

  readWordAndValue(infile, "preylengths", aggfilename);
  datafile.open(aggfilename, ios::in);
  handle.checkIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  i = readLengthAggregation(subdata, preylengths, preylenindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  LgrpDiv = new LengthGroupDivision(preylengths);
  if (LgrpDiv->Error())
    handle.logMessage(LOGFAIL, "Error in prey - failed to create length group");

  //read the energy content of this prey
  infile >> ws;
  char c = infile.peek();
  if ((c == 'e') || (c == 'E'))
    readWordAndVariable(infile, "energycontent", energy);
  else
    energy = 1.0;

  if (isZero(energy))
    handle.logMessage(LOGFAIL, "Error in prey - energy content must be non-zero");

  //read from file - initialise things
  int numlen = LgrpDiv->numLengthGroups();
  int numarea = areas.Size();
  PopInfo nullpop;

  preynumber.AddRows(numarea, numlen, nullpop);
  biomass.AddRows(numarea, numlen, 0.0);
  cons.AddRows(numarea, numlen, 0.0);
  consumption.AddRows(numarea, numlen, 0.0);
  isoverconsumption.resize(numarea, 0);
  total.resize(numarea, 0.0);
  ratio.AddRows(numarea, numlen, 0.0);
  consratio.AddRows(numarea, numlen, 0.0);
  overconsumption.AddRows(numarea, numlen, 0.0);

  //preylenindex is not required - free up memory
  for (i = 0; i < preylenindex.Size(); i++)
    delete[] preylenindex[i];
}

Prey::Prey(const DoubleVector& lengths, const IntVector& Areas,
  double Energy, const char* givenname)
  : HasName(givenname), LivesOnAreas(Areas), energy(Energy) {

  LgrpDiv = new LengthGroupDivision(lengths);
  if (LgrpDiv->Error())
    handle.logMessage(LOGFAIL, "Error in prey - failed to create length group");
  CI = new ConversionIndex(LgrpDiv, LgrpDiv);
  if (CI->Error())
    handle.logMessage(LOGFAIL, "Error in prey - error when checking length structure");

  int numlen = LgrpDiv->numLengthGroups();
  int numarea = areas.Size();
  PopInfo nullpop;

  preynumber.AddRows(numarea, numlen, nullpop);
  biomass.AddRows(numarea, numlen, 0.0);
  cons.AddRows(numarea, numlen, 0.0);
  consumption.AddRows(numarea, numlen, 0.0);
  isoverconsumption.resize(numarea, 0);
  total.resize(numarea, 0.0);
  ratio.AddRows(numarea, numlen, 0.0);
  consratio.AddRows(numarea, numlen, 0.0);
  overconsumption.AddRows(numarea, numlen, 0.0);
}

Prey::~Prey() {
  delete CI;
  delete LgrpDiv;
}

void Prey::setCI(const LengthGroupDivision* const GivenLDiv) {
  CI = new ConversionIndex(GivenLDiv, LgrpDiv);
  if (CI->Error())
    handle.logMessage(LOGFAIL, "Error in prey - error when checking length structure");
}

void Prey::Print(ofstream& outfile) const {
  int i, area;
  outfile << "\nPrey\n\tName " << this->getName() << "\n\tEnergy content " << energy << "\n\t";
  LgrpDiv->Print(outfile);
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tNumber of prey on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++)
      outfile << setw(smallwidth) << setprecision(smallprecision) << preynumber[area][i].N << sep;
    outfile << "\n\tWeight of prey on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++)
      outfile << setw(smallwidth) << setprecision(smallprecision) << preynumber[area][i].W << sep;
    outfile << "\n\tConsumption of prey on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++)
      outfile << setw(smallwidth) << setprecision(smallprecision) << consumption[area][i] << sep;
    outfile << "\n\tOverconsumption of prey on internal area " << areas[area] << ":\n\t";
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++)
      outfile << setw(smallwidth) << setprecision(smallprecision) << overconsumption[area][i] << sep;
    outfile << endl;
  }
}

//reduce the population of the stock by the consumption
void Prey::Subtract(AgeBandMatrix& Alkeys, int area) {
  Alkeys.Subtract(consratio[this->areaNum(area)], *CI);
}

//adds the consumption by biomass
void Prey::addBiomassConsumption(int area, const DoubleVector& predcons) {
  int i, inarea = this->areaNum(area);
  if (predcons.Size() != cons[inarea].Size())
    handle.logMessage(LOGFAIL, "Error in consumption - cannot add different size vectors");
  for (i = 0; i < predcons.Size(); i++)
    cons[inarea][i] += predcons[i];
}

//adds the consumption by numbers
void Prey::addNumbersConsumption(int area, const DoubleVector& predcons) {
  int i, inarea = this->areaNum(area);
  if (predcons.Size() != cons[inarea].Size())
    handle.logMessage(LOGFAIL, "Error in consumption - cannot add different size vectors");
  for (i = 0; i < predcons.Size(); i++)
    cons[inarea][i] += (predcons[i] * preynumber[inarea][i].W);
}

//check if more is consumed of prey than was available.  If this is
//the case a flag is set. Changed 22 - May 1997  so that only 95% of a prey
//in an area can be eaten in one timestep.  This is to avoid problems
//with survey indices etc.
void Prey::checkConsumption(int area, const TimeClass* const TimeInfo) {
  int i, over = 0;
  int inarea = this->areaNum(area);
  double maxRatio, rat;

  maxRatio = MaxRatioConsumed;
  if (TimeInfo->numSubSteps() != 1)
    maxRatio = pow(MaxRatioConsumed, TimeInfo->numSubSteps());

  for (i = 0; i < LgrpDiv->numLengthGroups(); i++) {
    rat = 0.0;
    if (biomass[inarea][i] > verysmall)
      rat = cons[inarea][i] / biomass[inarea][i];

    ratio[inarea][i] = rat;
    if (rat > maxRatio) {
      over = 1;
      overconsumption[inarea][i] += (rat - maxRatio) * biomass[inarea][i];
      consratio[inarea][i] = 1.0 - maxRatio;
      cons[inarea][i] = biomass[inarea][i] * maxRatio;
    } else {
      consratio[inarea][i] = 1.0 - rat;
    }

    consumption[inarea][i] += cons[inarea][i];
  }
  isoverconsumption[inarea] = over;
}

void Prey::Reset() {
  int i, area;
  for (area = 0; area < areas.Size(); area++) {
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++) {
      consumption[area][i] = 0.0;
      overconsumption[area][i] = 0.0;
    }
  }
  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Reset consumption data for prey", this->getName());
}

int Prey::isPreyArea(int area) {
  if (this->isInArea(area) == 0)
    return 0;
  if (total[this->areaNum(area)] < 0.0)
    handle.logMessage(LOGWARN, "Warning in prey - negative amount consumed");
  if (isZero(total[this->areaNum(area)]))
    return 0;
  return 1;
}

int Prey::isOverConsumption(int area) {
  if (this->isInArea(area) == 0)
    return 0;
  return isoverconsumption[this->areaNum(area)];
}
