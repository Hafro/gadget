#ifndef suitfuncptrvector_h
#define suitfuncptrvector_h

#include "suitfunc.h"

class SuitFunc;

/**
 * \class SuitFuncPtrVector
 * \brief This class implements a dynamic vector of SuitFunc values
 */
class SuitFuncPtrVector {
public:
  /**
   * \brief This is the default SuitFuncPtrVector constructor
   */
  SuitFuncPtrVector() { size = 0; v = 0; };
  /**
   * \brief This is the SuitFuncPtrVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~SuitFuncPtrVector();
  /**
   * \brief This will add new entries to the vector
   * \param add is the number of new entries to the vector
   * \param suitf is the value that will be entered for the new entries
   */
  void resize(int add, SuitFunc* suitf);
  /**
   * \brief This will add new empty entries to the vector
   * \param add is the number of new entries to the vector
   * \note The new elements of the vector will be created, and set to zero
   */
  void resize(int add);
  /**
   * \brief This will delete an entry from the vector
   * \param pos is the element of the vector to be deleted
   * \param keeper is the Keeper for the current model
   * \note This will free the memory allocated to the deleted element of the vector
   */
  void Delete(int pos, Keeper* const keeper);
  /**
   * \brief This will return the size of the vector
   * \return the size of the vector
   */
  int Size() const { return size; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  SuitFunc*& operator [] (int pos) { return v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  SuitFunc* const& operator [] (int pos) const { return v[pos]; };
  /**
   * \brief This function will read SuitFunc data from the input file
   * \param infile is the CommentStream to read suitability function data from
   * \param name is the name for the suitability function
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \return 1 if the suitability function has been read from file, 0 otherwise
   */
  int readSuitFunction(CommentStream& infile, const char* suitfname,
    const TimeClass* const TimeInfo, Keeper* const keeper);
protected:
  /**
   * \brief This is the vector of SuitFunc values
   */
  SuitFunc** v;
  /**
   * \brief This is the size of the vector
   */
  int size;
};

#endif
