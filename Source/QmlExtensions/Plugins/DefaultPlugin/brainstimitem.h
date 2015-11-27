//DefaultQMLPlugin
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

#ifndef BRAINSTIMITEM_H
#define BRAINSTIMITEM_H

#include <QtQuick/QQuickItem>
#include <QtCore/QPropertyAnimation>
#include <QDir>

//!  The BrainStimItem class. 
/*!
  The BrainStimItem Class implements a QML BrainStimItem
*/
class BrainStimItem : public QQuickItem
{
    Q_OBJECT
	//! \brief camera property.
	/*!  Use this property to access the OgreCameraNode object.
    */
   // Q_PROPERTY(QObject *camera READ camera)

//signals:
	//! \brief ogreNodeInitialized() signal.
	/*!  This signal is emitted just after the Ogre3D engine is initialized.
	 */
//	void ogreNodeInitialized();
	//! \brief ogreNodePreInitialize() signal.
	/*!  This signal is emitted just before the Ogre3D engine is initialized.
	 */
//	void ogreNodePreInitialize();

public:
    BrainStimItem(QQuickItem *parent = 0);
	 ~BrainStimItem();
    //QObject *camera() const { return m_camera; }

public slots:
	//! \brief addResourceLocation slot.
	/*!  This function can add/append a specific resource location to the Ogre3D engine, just before it initialized.
	 * @param sLocation a string containing the path to the resource location
	 * @param sType a string containing the type of the resource location this can be "FileSystem" or "Zip"
	 * @return a boolean value determining whether the function executed successfully.
	 */	
	//QString getCurrentDirectory();	
};

#endif // BRAINSTIMITEM_H
