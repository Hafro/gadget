#include "mortpredator.h"
#include "keeper.h"
#include "prey.h"
#include "mathfunc.h"
#include "mortprey.h"
#include "errorhandler.h"
#include "gadget.h"

//written by kgf 20/5 98 on the basis of linearpredator
MortPredator::MortPredator(CommentStream& infile, const char* givenname, const IntVector& Areas,
  const LengthGroupDivision* const OtherLgrpDiv, const LengthGroupDivision* const GivenLgrpDiv,
  const TimeClass* const TimeInfo, Keeper* const keeper)
  : LengthPredator(givenname, Areas, OtherLgrpDiv, GivenLgrpDiv, 1.0),
  effort(Areas.Size(), TimeInfo->TotalNoSteps(), 0.),
  f_level(Areas.Size(), TimeInfo->TotalNoSteps(), 0.) {

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int areanr = Areas.Size();
  int i;

  keeper->AddString("predator");
  keeper->AddString(givenname);
  pres_time_step = TimeInfo->CurrentTime();
  no_of_time_steps = TimeInfo->TotalNoSteps();

  this->ReadSuitabilityMatrix(infile, "calcflev", TimeInfo, keeper);
  keeper->ClearLast();
  keeper->ClearLast();
  //Predator::SetPrey will call ResizeObjects.

  infile >> calc_f_lev;
  if (calc_f_lev == 1) { //f_level is calculated as a function
    year0 = new int[areanr];
    year = new int[areanr];
    for (i = 0; i < areanr; i++)
      infile >> year0[i];
    for (i = 0; i < areanr; i++)
      infile >> year[i];
    for (i = 0; i < areanr; i++) {
      q1.resize(1, keeper);
      infile >> q1[i];
      q1[i].Inform(keeper);
      q2.resize(1, keeper);
      infile >> q2[i];
      q2[i].Inform(keeper);
    }
    //JMB - taken out of for loop - only read matrix effort once!
    readMatrix(infile, effort); //read effort from file
    calcFlevel();

  } else { //f_lev is read from file, q1, q2 and effort not used
    year0 = 0;
    year = 0;
    f_lev.AddRows(areanr, no_of_time_steps);
    for (i = 0; i < areanr; i++) {
      infile >> f_lev[i];
      f_lev[i].Inform(keeper);
    }
  }
  c_hat.AddRows(areanr, NoPreys());

  //prepare far reading the amounts
  infile >> text >> ws;
  if (!(strcasecmp(text, "amount") == 0))
    handle.Unexpected("amount", text);
}

MortPredator::~MortPredator() {
  delete[] year;
  delete[] year0;
}

void MortPredator::Eat(int area, double LengthOfStep, double Temperature, double Areasize,
  int CurrentSubstep, int NrOfSubsteps) {

  //Written by kgf 20/5 98
  //The parameters LengthOfStep, Temperature and Areasize will not be used.
  int inarea = AreaNr[area];
  int prey, predl, preyl;
  double time_frac, pres_f_lev;

  if (CurrentSubstep == 1) { //Fishing mortality constant in a time step

    if (NrOfSubsteps > 1)
      time_frac = double(1.0 / NrOfSubsteps);
    else
      time_frac = 1.0;

    if (calc_f_lev == 1)
      pres_f_lev = f_level[inarea][pres_time_step - 1] * time_frac;
    else
      pres_f_lev = f_lev[inarea][pres_time_step - 1] * time_frac;

    //calculate consumption
    //JMB 12/NOV/01 removed the following loop - repeated over predl??
    //for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++)
    for (prey = 0; prey < NoPreys(); prey++) {
      if (Preys(prey)->IsInArea(area)) {
        for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++) {
          for (preyl = Suitability(prey)[predl].Mincol();
              preyl < Suitability(prey)[predl].Maxcol(); preyl++) {
            consumption[inarea][prey][predl][preyl] =
              pres_f_lev * Suitability(prey)[predl][preyl];
          }
        }

      } else {
        for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++) {
          for (preyl = Suitability(prey)[predl].Mincol();
              preyl < Suitability(prey)[predl].Maxcol(); preyl++)
            consumption[inarea][prey][predl][preyl] = 0;
        }
      }
    }

    //Inform the preys of the fleet's fishing mortality.
    for (prey = 0; prey < NoPreys(); prey++) {
      if (Preys(prey)->IsInArea(area)) {
        for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++)
          Preys(prey)->AddConsumption(area, consumption[inarea][prey][predl]);
      }
    }
    pres_time_step++;
  }
}

void MortPredator::calcCHat(int area, const TimeClass* const TimeInfo) {
  //written by kgf 22/5 98
  //modified by kgf 14/7 98
  //This routine supposes that the total mortality (included natural mortality)
  //is calculated before this routine is called.
  //It assumes that mean_n (middle number over the (sub)timestep) is
  //calculated in MortPrey.

  int prey, predl, preyl, age, minl, maxl;
  int timestep = TimeInfo->CurrentTime() - 1;
  int inarea = AreaNr[area];
  double time_frac, pres_f_lev;

  if (TimeInfo->NrOfSubsteps() > 1)
    time_frac = double(1.0 / (TimeInfo->NrOfSubsteps()));
  else
    time_frac = 1.0;
  if (calc_f_lev == 1)
    pres_f_lev = f_level[inarea][timestep] * time_frac;
  else
    pres_f_lev = f_lev[inarea][timestep] * time_frac;

  if (!initialisedCHat) {
    for (prey = 0; prey < NoPreys(); prey++) {
      if (Preys(prey)->IsInArea(area)) {
        //set correct dimensions in c_hat
        this->InitializeCHat(area, prey, ((MortPrey*)Preys(prey))->getMeanN(area));
      }
    }
    initialisedCHat = 1;
  }

  if (TimeInfo->CurrentSubstep() == 1) {
    for (prey = 0; prey < NoPreys(); prey++) {
      if (Preys(prey)->IsInArea(area)) {
        for (age = ((MortPrey*)Preys(prey))->getMeanN(area).Minage();
            age <= ((MortPrey*)Preys(prey))->getMeanN(area).Maxage(); age++) {
          for (preyl = ((MortPrey*)Preys(prey))->getMeanN(area).Minlength(age);
              preyl < ((MortPrey*)Preys(prey))->getMeanN(area).Maxlength(age); preyl++)
            c_hat[inarea][prey][age][preyl] = 0.0;
        }
      }
    }
  }

  for (prey = 0; prey < NoPreys(); prey++) {
    if (Preys(prey)->IsInArea(area)) {
      if (pres_f_lev > rathersmall) {
        for (age = ((MortPrey*)Preys(prey))->getMeanN(area).Minage();
            age <= ((MortPrey*)Preys(prey))->getMeanN(area).Maxage(); age++) {
          for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++) {
            minl = max(((MortPrey*)Preys(prey))->getMeanN(area).Minlength(age),
              Suitability(prey)[predl].Mincol());
            maxl = min(((MortPrey*)Preys(prey))->getMeanN(area).Maxlength(age),
              Suitability(prey)[predl].Maxcol());
            for (preyl = minl; preyl < maxl; preyl++) {
              c_hat[inarea][prey][age][preyl] +=
                consumption[inarea][prey][predl][preyl] *
                ((MortPrey*)Preys(prey))->getMeanN(area)[age][preyl].N; //F*mean_n
            }
          }
        }
      }
    }
  }
}

const double MortPredator::consumedBiomass(int prey_nr, int area_nr) const {

  double tons = 0.0;
  int age, len;
  const AgeBandMatrix& mean = ((MortPrey*)Preys(prey_nr))->getMeanN(area_nr);
  for (age = mean.Minage(); age <= mean.Maxage(); age++)
    for (len = mean.Minlength(age); len < mean.Maxlength(age); len++)
      tons += c_hat[area_nr][prey_nr][age][len] * mean[age][len].W;

  return tons;
}

void MortPredator::Print(ofstream& outfile) const {
  //Denne kan Morten modifisera!!
  outfile << "MortPredator\n";
  PopPredator::Print(outfile);
}

const PopInfoVector& MortPredator::NumberPriortoEating(int area, const char* preyname) const {
  int prey;
  for (prey = 0; prey < NoPreys(); prey++)
    if (strcasecmp(Preyname(prey), preyname) == 0)
      return Preys(prey)->NumberPriortoEating(area);

  cerr << "Predator " << this->Name() << " was asked for consumption\n"
    << "of prey " << preyname << " which he does not eat\n";
  exit(EXIT_FAILURE);
}

double MortPredator::getFlevel(int area, const TimeClass* const TimeInfo) {
  //written by kgf 16/9 98
  double pres_f_lev = 0.0;
  int inarea = AreaNr[area];
  if (calc_f_lev == 1)  //f_level calculated from effort
    pres_f_lev = f_level[inarea][TimeInfo->CurrentTime() - 1];
  else //f_level read from file, may be optimized
    pres_f_lev = f_lev[inarea][TimeInfo->CurrentTime() - 1];
  return pres_f_lev;
}

void MortPredator::calcFlevel() {
  //written by kgf 18/6 98
  //f_level = (q1 + q2 * (year - year0)) * effort
  int area, inarea, j;
  for (area = 0; area < areas.Size(); area++) {
    inarea = AreaNr[area];
    for (j = 0; j < no_of_time_steps; j++)
      f_level[inarea][j] = (q1[area] + q2[area] *
        (year[area] - year0[area])) * effort[area][j];
  }
}

void MortPredator::InitializeCHat(int area, int prey, const AgeBandMatrix& mean_n) {
  //written by kgf 14/7 98
  int inarea = AreaNr[area];
  int i = 0;
  int age;
  IntVector size(mean_n.Nrow());
  IntVector minl(mean_n.Nrow());
  for (age = mean_n.Minage(); age <= mean_n.Maxage(); age++) {
    size[i] = mean_n.Maxlength(age) - mean_n.Minlength(age) + 1;
    minl[i] = mean_n.Minlength(age);
    i++;
  }
  assert(i == size.Size());
  BandMatrix tmp(minl, size, mean_n.Minage(), 0.0);
  c_hat.ChangeElement(inarea, prey, tmp);
}

void MortPredator::Multiply(AgeBandMatrix& stock_alkeys, const DoubleVector& ratio) {
  //written by kgf 31/7 98
  //Note! ratio is supposed to have equal dimensions to MortPredator.
  stock_alkeys.Multiply(ratio, *CI);
}

void MortPredator::Reset(const TimeClass* const TimeInfo) {
  //written by kgf 21/9 98
  this->PopPredator::Reset(TimeInfo);
  if (TimeInfo->CurrentTime() == 1)
    pres_time_step = 1;
  initialisedCHat = 0;
}