//ExperimentManagerplugin
//Copyright (C) 2015  Sven Gijsen
//
//This file is part of BrainStim.
//BrainStim is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.
//


#ifndef RANDOMGENERATOR_H
#define RANDOMGENERATOR_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QtScript/QtScript>
#include <QtScript/QScriptEngine>
#include "randomc/randomc.h"

#ifdef Q_OS_WIN32 //Are we on Windows?
	#include "windows.h"
#endif

#define RANDOMIZE_DATETIME_FORMAT			"HHmmsszzz"
//RANDOMGENERATOR_NAME

// This template class combines two different random number generators
// for improved randomness. R1 and R2 are any two different random number
// generator classes.
template <class RG1, class RG2>
class TRandomCombined : private RG1, private RG2 
{
public:
	TRandomCombined(int seed) : RG1(seed), RG2(seed+1) {};

	void RandomInit(int seed) {        // re-seed
		RG1::RandomInit(seed);
		RG2::RandomInit(seed+1);
	}

	double Random() {
		double r = RG1::Random() + RG2::Random();
		if (r >= 1.) r -= 1.;
		return r;
	}

	int IRandom(int min, int max)
	{// output random integer
		if(min == max)
			return min;
		// get integer random number in desired interval
		int iinterval = max - min;// + 1;
		if (iinterval < 0)//<= 0) 
			return 0x80000000; // error
		int r = int((iinterval+1) * Random()); // truncate, Random() returns <1.0
		//if (r >= iinterval) 
		//	return max;//r = iinterval-1;
		return min + r;
	}

	double DRandom(double min, double max)
	{// output random double
		if(min == max)
			return min;
		// get double random number in desired interval
		double iinterval = max - min;// + 1;
		if (iinterval <= 0)//<= 0) 
			return 0x80000000; // error
		//double dsmall = DBL_MIN;
		double r = (iinterval+DBL_MIN) * Random(); // truncate
		//if (r >= iinterval) 
		//	return max;//	r = iinterval-1;
		return min + r;
	}
};

//!  The RandomGenerator class. 
/*!
The Random Generator can be used to create and edit randomized lists and numbers. For the randomization it combines two 
different random number generators (Mersenne twister and Mother-of-All (Multiply with carry).
*/
class RandomGenerator : public QObject, public QStringList
{
	Q_OBJECT

public:
	RandomGenerator(QObject *parent = NULL);
	RandomGenerator(const RandomGenerator& other ){Q_UNUSED(other);};//TODO fill in copy constructor, should be declared for the Q_DECLARE_METATYPE macro
	~RandomGenerator();

	/*! 
		* The enum (RandomGenerator_RandomizeMethod) can store a Randomization method 
	*/
	enum RandomGenerator_RandomizeMethod
	{
		RandomGenerator_RandomizeStandard						= 0, //!< 0: Standard randomization, it combines two different random number generators (Mersenne twister and Mother-of-All (Multiply with carry).
		RandomGenerator_RandomizePreservedIndexes				= 1, //!< 1: Standard randomization where a custom array can be provided that contains the indexes of the values inside the randomization array that are forced to be present in the new randomized array. These values of those indexes are thus preserved but their positions (indexes) are randomized.
		RandomGenerator_RandomizePreservedValues				= 2, //!< 2: Standard randomization where a custom array can be provided that contains the values inside the randomization array that are forced to be present in the new randomized array. These values are thus preserved but their positions (indexes) are randomized.
		RandomGenerator_RandomizePreservedNonRandomizedIndexes	= 3  //!< 3: The same as RandomGenerator::RandomGenerator_RandomizePreservedIndexes, the values of the indexes are also preserved but their positions (indexes) are NOT randomized.
	};

	static QScriptValue ctor__randomGenerator(QScriptContext* context, QScriptEngine* engine);

	bool randomizeList(RandomGenerator_RandomizeMethod rMethod = RandomGenerator_RandomizeStandard, QStringList *sList = NULL);		//Randomizes the class inherited QStringList
	void setScriptEngine(QScriptEngine* engine);

public slots:
	bool makeThisAvailableInScript(QString strObjectScriptName = "", QObject *engine = NULL);//To make the objects (e.g. defined in a *.exml file) available in the script

	/*! \brief Creates a random Integer value.
	*
	*  This function creates a random integer value between (and including) a defined range.
	* @param nMin an integer value that determines the minimal integer value of the range where a random number is picked from.
	* @param nMax an integer value that determines the maximal integer value of the range where a random number is picked from.
	* @return an random integer value between (and including) a defined range. If the range is not correct it returns 0x80000000;
	*/
	int randomizeInt(int nMin, int nMax);
	/*! \brief Creates a random Double value.
	*
	*  This function creates a random double value between (and including) a defined range.
	* @param dMin an integer value that determines the minimal double value of the range where a random number is picked from.
	* @param dMax an integer value that determines the maximal double value of the range where a random number is picked from.
	* @return an random integer value between (and including) a defined range. If the range is not correct it returns 0x80000000;
	*/
	double randomizeDouble(double dMin, double dMax);
	/*! \brief Returns the String List.
	* 
	* This function returns the String List.
	* @return the StringList (array).
	*/
	QStringList toStringList();
	/*! \brief Randomizes and return the String List.
	* 
	* This function randomizes and then returns the randomized String List.
	* @param rMethod an integer value representing the randomization method to use, see RandomGenerator::RandomGenerator_RandomizeMethod.
	* @param sList an optional array that represents the items to be preserved, see RandomGenerator::RandomGenerator_RandomizeMethod.
	* @return a randomized StringList (array).
	*/
	QStringList randomize(int rMethod  = (int)RandomGenerator_RandomizeStandard, QStringList sList = QStringList());
	/*! \brief Returns the String at index position i in the StringList.
	*
	*  This function returns the String at index position i in the list, 
	*  i must be a valid index position in the list (i.e., 0 <= i < RandomGenerator::count()).
	*  This function is very fast (constant time).
	* @param i an integer value that holds the index position.
	* @return an String value containing the item at index position in the list.
	*/
	const QString at(int i) const;
	/*! \brief Returns the number of String items in the list.
	*
	* @return an integer value containing the number of String items in the list.
	*/
	int count() const;
	/*! \brief Returns the number of a particular String occurrences in the list.
	*
	* @param sValue an String value that holds the String to search for in the list.
	* @return an integer value containing the number of a particular String occurrences in the list.
	*/
	int count(const QString &sValue) const;
	/*! \brief Returns true if the list contains no Strings; otherwise returns false.
	*
	* @return an boolean value representing whether the list was empty or not.
	*/
	bool isEmpty() const;
	/*! \brief Inserts a defined String at the end of the list.
	*
	* This operation is typically very fast (constant time), because the list preallocates extra space on both sides of its internal buffer to allow for fast growth at both ends of the list.
	* @param sValue an String value that should be inserted at the end of the list.
	*/
	void append(const QString &sValue);
	/*! \brief Inserts a defined StringList at the end of the list.
	*
	* This function inserts a defined StringList to the end of the list and returns the number of Strings in the list.
	* @param sList an StringList (array) that should be inserted at the end of the list.
	* @return an integer value representing the number of Strings in the list.
	*/
	int appendStringList(const QStringList &sList);
	/*! \brief Removes the first String in the list and returns it.
	* 
	* This function assumes the list is not empty. To avoid failure, call RandomGenerator::isEmpty() before calling this function. This operation takes constant time.
	* @return a String value containing the removed String from the list.
	*/
	QString takeFirst();
	/*! \brief Removes the last String in the list and returns it.
	* 
	* This function assumes the list is not empty. To avoid failure, call RandomGenerator::isEmpty() before calling this function. This operation takes constant time.
	* @return a String value containing the removed String from the list.
	*/
	QString takeLast();
	/*! \brief Removes the String at a defined index position in the list and returns it.
	* 
	* This function assumes the list is not empty. To avoid failure, call RandomGenerator::isEmpty() before calling this function. This operation takes constant time.
	* @param i an integer value holding the index position.
	* @return a String value containing the removed String from the list.
	*/
	QString takeAt(int i);
	/*! \brief Removes all items from the list.
	*
	* This function removes all items from the list.
	*/
	void clear();
	/*! \brief Removes all occurrences of a String in the list and returns the number of entries removed.
	* 
	* @param sValue the String value that should be removed (all) from the list.
	* @return an integer value representing the number of entries that were removed.
	*/
	int removeAll(const QString &sValue);
	/*! \brief Removes the String at a defined index position in the list.
	* 
	* This function assumes the list is not empty. To avoid failure, call RandomGenerator::isEmpty() before calling this function. This operation takes constant time.
	* @param i an integer value holding the index position.
	*/
	void removeAt(int i);
	/*! \brief Returns the index position of the first occurrence of the defined String in the list.
	* 
	* This function Returns the index position of the first occurrence of the defined String in the list, searching forward from index position from. Note that QList uses 0-based indexes.
	* @param sValue the String value to search for in the list.
	* @nFrom an integer value holding the starting index position for the search operation. This value is 0 if not defined.
	* @return an integer holding the index of the first occurrence of the defined String in the list. This function returns -1 if no item matched.
	*/
	int indexOf(const QString &sValue, int nFrom = 0) const;
	/*! \brief Exchange the String value at index position i with the String value at index position j.
	* 
	* This function assumes that both i and j are at least 0 but less than RandomGenerator::count(). To avoid failure, test that both i and j are at least 0 and less than RandomGenerator::count().
	* @param i an integer value holding the first index position.
	* @param j an integer value holding the second index position.
	*/
	void swap(int i, int j);
	/*! \brief Inserts a defined String value at specific index position i.
	* 
	* This function inserts a String value at index position i in the list. If i is 0, the value is prepended to the list. If i is RandomGenerator::count(), the value is appended to the list.
	* @param i an integer value holding the index position.
	* @param sValue the String value to insert to the list.
	*/
	void insert(int i, const QString &sValue);

private:
	QScriptEngine* currentScriptEngine;
	TRandomCombined<CRandomMersenne,CRandomMother> *tCombinedRandGen;
	int nSeed;
};

Q_DECLARE_METATYPE(RandomGenerator*)

#endif // RANDOMGENERATOR_H
