#include "popstatistics.h"
#include "popinfo.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"
#include "agebandmatrix.h"

void PopStatistics::calcStatistics(const PopInfoIndexVector& pop,
  const LengthGroupDivision* const lgrpdiv, int calcweight) {

  PopInfo sum;
  int i, offset; 
  double tmp;

  if (pop.Size() != lgrpdiv->numLengthGroups())
    handle.logMessage(LOGFAIL, "Error in popstatistics - length groups dont match population");

  offset = pop.minCol();
  meanlength = meanweight = totalnumber = sdevlength = 0.0;
  for (i = offset; i < pop.maxCol(); i++) {
    if ((handle.getLogLevel() >= LOGWARN) && calcweight)
      if ((isZero(pop[i].W)) && (!(isZero(pop[i].N))))
        handle.logMessage(LOGWARN, "Warning in popstatistics - non-zero population has zero mean weight");

    if (calcweight)
      sum += pop[i];
    totalnumber += pop[i].N;
    meanlength += pop[i].N * lgrpdiv->meanLength(i - offset);
  }

  if (totalnumber > verysmall) {
    if (calcweight)
      meanweight = sum.W;
    meanlength /= totalnumber;
    for (i = offset; i < pop.maxCol(); i++) {
      tmp = meanlength - lgrpdiv->meanLength(i - offset);
      sdevlength += pop[i].N * tmp * tmp;
    }
    sdevlength = sqrt(sdevlength / totalnumber);
  } else  {
    //JMB reset back to 0
    meanlength = 0.0;
    totalnumber = 0.0;
  }
}

void PopStatistics::calcStatistics(const AgeBandMatrix& agelenum, int lengr) {

  int age;
  
// loop over the ages to find specific length group else go to next age

  meanlength = meanweight = totalnumber = sdevlength = 0.0;
  for (age = agelenum.minAge(); age <= agelenum.maxAge(); age++) {
    if ((handle.getLogLevel() >= LOGWARN))
      if ((isZero((agelenum[age][lengr]).W)) && (!((agelenum[age][lengr]).N)))
        handle.logMessage(LOGWARN, "Warning in popstatistics - non-zero population has zero mean weight");

    meanweight += (agelenum[age][lengr]).W;
    totalnumber += (agelenum[age][lengr]).N;
  }

  if (totalnumber > verysmall) {
    meanweight /= totalnumber;
    sdevlength = sqrt(sdevlength / totalnumber);
  } else {
    meanweight = 0.0;
    totalnumber = 0.0;
  }
}




