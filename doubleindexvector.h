#ifndef doubleindexvector_h
#define doubleindexvector_h

/**
 * \class doubleindexvector
 * \brief This class implements a dynamic vector of double values, indexed from minpos not 0
 */
class doubleindexvector {
public:
  /**
   * \brief This is the default doubleindexvector constructor
   */
  doubleindexvector() { minpos = 0; size = 0; v = 0; };
  /**
   * \brief This is the doubleindexvector constructor for a specified size
   * \param sz this is the size of the vector to be created
   * \param minpos this is the index for the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  doubleindexvector(int sz, int minpos);
  /**
   * \brief This is the doubleindexvector constructor for a specified size with an initial value
   * \param sz this is the size of the vector to be created
   * \param minpos this is the index for the vector to be created
   * \param initial this is the initial value for all the entries of the vector
   */
  doubleindexvector(int sz, int minpos, double initial);
  /**
   * \brief This is the doubleindexvector constructor that create a copy of an existing doubleindexvector
   * \param initial this is the doubleindexvector to copy
   */
  doubleindexvector(const doubleindexvector& initial);
  /**
   * \brief This is the doubleindexvector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~doubleindexvector();
  /**
   * \brief This will add new entries to the vector
   * \param addsize this is the number of new entries to the vector
   * \param newminpos this is the index for the vector to be created
   * \param value this is the value that will be entered for the new entries
   */
  void resize(int addsize, int newminpos, double value);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  double& operator [] (int pos);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  const double& operator [] (int pos) const;
  /**
   * \brief This will return the index of the vector
   * \return the index of the vector
   */
  int Mincol() const { return minpos; };
  /**
   * \brief This will return the length of the vector
   * \return the length of the vector
   */
  int Maxcol() const { return minpos + size; };
  /**
   * \brief This will return the size of the vector
   * \return the size of the vector
   */
  int Size() const { return size; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  void Delete(int pos);
protected:
  /**
   * \brief This is index for the vector
   */
  int minpos;
  /**
   * \brief This is size of the vector
   */
  int size;
  /**
   * \brief This is the indexed vector of double values
   */
  double* v;
};

#ifdef GADGET_INLINE
#include "doubleindexvector.icc"
#endif

#endif
