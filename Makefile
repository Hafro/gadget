##########################################################################
# Common for all architecture and compiler options
##########################################################################
GCCWARNINGS = -Wimplicit -Wreturn-type -Wswitch -Wcomment -Wformat \
              -Wparentheses -Wpointer-arith -Wcast-qual -Wconversion \
              -Wreorder -Wwrite-strings -Wsynth -Wchar-subscripts \
              -Wuninitialized -W -pedantic

#DEFINE_FLAGS = -D DEBUG -D INTERRUPT_HANDLER -g -O
DEFINE_FLAGS = -D NDEBUG -D INTERRUPT_HANDLER -O2 -s

##########################################################################
# The name of the final executable (eg gadget-paramin or gadget.exe)
GADGET = gadget
##########################################################################
# Pick the appropriate compiler from the following switches
##########################################################################
# 1. Linux, or Cygwin, or Solaris, without pvm3, g++ compiler
CXX = g++
LIBDIRS = -L. -L/usr/local/lib
LIBRARIES = -lm -lvec
CXXFLAGS = $(GCCWARNINGS) $(DEFINE_FLAGS)
OBJECTS = $(GADGETOBJECTS)
##########################################################################
# 2. Linux, or Cygwin, or Solaris, with pvm3, g++ compiler
#CXX = g++
#PVMDIR = $(PVM_ROOT)
#PVMINCLUDE = $(PVMDIR)/include
#PVMLIB = $(PVMDIR)/lib/$(PVM_ARCH)
#LIBDIRS = -L. -L/usr/local/lib -L$(PVMLIB)
#LIBRARIES = -lm -lvec -lpvm3 -lnsl
## note that some (older) linux machines also require the -lsocket library
#CXXFLAGS = $(GCCWARNINGS) $(DEFINE_FLAGS) -D GADGET_NETWORK -I$(PVMINCLUDE)
#OBJECTS = $(GADGETOBJECTS) $(SLAVEOBJECTS)
##########################################################################
# 3. Solaris, without pvm3, using CC compiler
#CXX = CC
#LIBDIRS = -L. -L/usr/local/lib
#LIBRARIES = -lm -lvec
#CXXFLAGS = $(DEFINE_FLAGS)
#OBJECTS = $(GADGETOBJECTS)
##########################################################################
# 4. Linux or Solaris, without pvm3, g++ compiler running CONDOR
#CXX = condor_compile g++
#LIBDIRS = -L. -L/usr/local/lib
#LIBRARIES = -lm -lvec
#CXXFLAGS = $(GCCWARNINGS) $(DEFINE_FLAGS)
#OBJECTS = $(GADGETOBJECTS)
##########################################################################
# 5. Windows using services for unix
#CXX = g++
#LIBDIRS = -L. -L/usr/local/lib
#LIBRARIES = -lm -lvec
#CXXFLAGS = $(GCCWARNINGS) $(DEFINE_FLAGS)
#OBJECTS = $(GADGETOBJECTS)
#CPPFLAGS = -I/usr/local/include #-I/dev/fs/C/x/SDK/opt/gcc.3.3/include/c++/3.3/backward
##########################################################################

VECTORS = addresskeepermatrix.o addresskeepervector.o intmatrix.o intvector.o \
	formulamatrixptrvector.o doublematrix.o doublevector.o parametervector.o \
	otherfoodptrvector.o baseclassptrvector.o popinfoindexvector.o \
	conversionindexptrvector.o likelihoodptrvector.o suitfuncptrvector.o  \
	popratiomatrix.o popratiovector.o popinfomatrix.o popinfovector.o \
	doubleindexvector.o predatorptrvector.o preyptrvector.o fleetptrvector.o \
	doublematrixptrmatrix.o doublematrixptrvector.o printerptrvector.o \
	agebandmatrixptrvector.o popratioindexvector.o agebandmatrixptrmatrix.o \
	formulamatrix.o formulavector.o charptrmatrix.o charptrvector.o \
	timevariablevector.o stockptrvector.o agebandmatrixratioptrvector.o \
	tagptrvector.o bandmatrixptrvector.o rectangleptrvector.o migrationareaptrvector.o

GADGETOBJECTS = gadget.o parameter.o growermemberfunctions.o predatoraggregator.o \
	abstrpredstdinfo.o abstrpreystdinfo.o catchdistribution.o errorhandler.o \
	predatorprinter.o abstrpreystdinfobylength.o predatoroveraggregator.o \
	actionattimes.o predatoroverprinter.o strstack.o ecosystem.o poppredator.o \
	predpreystdageprinter.o agebandmatrix.o predpreystdlengthprinter.o grow.o \
	agebandmatrixmemberfunctions.o predpreystdprinter.o area.o predstdinfo.o \
	bandmatrix.o predstdinfobylength.o prey.o popstatistics.o ludecomposition.o \
	preyoveraggregator.o lengthgroup.o preyoverprinter.o catchstatistics.o \
	preystdinfo.o preystdinfobylength.o timevariable.o boundlikelihood.o stray.o \
	commentstream.o printinfo.o readfunc.o conversionindex.o readmain.o keeper.o \
	fleet.o sibyageonstep.o fleetpreyaggregator.o sibyfleetonstep.o predator.o \
	sibylengthonstep.o formula.o grower.o growthcalc.o simulation.o hooke.o \
	simann.o sionstep.o initialcond.o spawner.o initialize.o stochasticdata.o \
	lengthpredator.o stockaggregator.o lengthprey.o surveydistribution.o bfgs.o \
	stockdistribution.o linearpredator.o stockfullprinter.o linearregression.o \
	stockmemberfunctions.o livesonareas.o stockpredator.o stockpredstdinfo.o \
	loglinearregression.o time.o stockprey.o maininfo.o stockpreyfullprinter.o \
	stockpreystdinfo.o stockpreystdinfobylength.o stockprinter.o readword.o \
	stockstdprinter.o maturity.o suitfunc.o suits.o stomachcontent.o runid.o \
	migration.o recaggregator.o surveyindices.o migrationpenalty.o recapture.o \
        stock.o readaggregation.o multinomial.o totalpredator.o understocking.o \
        recstatistics.o transition.o otherfood.o selectfunc.o optinfo.o naturalm.o \
        likelihoodprinter.o catchinkilos.o interruptinterface.o interrupthandler.o \
        tags.o renewal.o taggrow.o initialinputfile.o optinfohooke.o optinfobfgs.o \
        optinfosimann.o popratio.o popinfo.o popinfomemberfunctions.o summaryprinter.o \
        numberpredator.o agebandmatrixratio.o agebandmatrixratiomemberfunctions.o \
        rectangle.o migrationarea.o

SLAVEOBJECTS = netdata.o slavecommunication.o pvmconstants.o

GADGETINPUT = intvector.o doublevector.o intmatrix.o doublematrix.o \
        initialinputfile.o charptrvector.o commentstream.o \
        parameter.o parametervector.o errorhandler.o strstack.o

LDFLAGS = $(CXXFLAGS) $(LIBDIRS) $(LIBRARIES)

gadget	:	$(OBJECTS) libvec.a
	$(CXX) -o $(GADGET) $(OBJECTS) $(LDFLAGS)

libvec.a:	$(VECTORS)
	ar rs libvec.a $?

## you need root permission to be able to do this ...
install	:	$(GADGET)
	strip $(GADGET)
	cp $(GADGET) /usr/local/bin/
	cp gadget.1 /usr/local/man/man1/

##########################################################################
# The following line is needed to create the gadget input library
# for newer versions of paramin.  To create this library, you need
# to type "make libgadgetinput.a" *before* you compile paramin
##########################################################################
libgadgetinput.a: $(GADGETINPUT)
	ar rs libgadgetinput.a $?

clean	:
	rm -f $(OBJECTS) $(VECTORS) libvec.a libgadgetinput.a

depend	:
	$(CXX) -M -MM $(CXXFLAGS) *.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
