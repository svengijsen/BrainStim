//parallelportplugin
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


#include "pport_dialog.h"

PPort_Dialog::PPort_Dialog(QWidget *parent)	: QDialog(parent)
{
	ui.setupUi(this);
	captureThreadRunning = false;
	generateThreadRunning = false;
	intValidator = new QIntValidator( 0, 99999, this );
	ui.txtLHDelay_C->setValidator( intValidator );
	ui.txtHLDelay_C->setValidator( intValidator );
	ui.txtPulseActiveTime_G->setValidator( intValidator );
	ui.txtRepetitionTime_G->setValidator( intValidator );

	getATBaseAddress();
	getCBaseAddress();
	getGBaseAddress();
}

PPort_Dialog::~PPort_Dialog()
{

}

short _httoi(const QString Qvalue)
{
	struct CHexMap
	{
		QString chr;
		int value;
	};
	const int HexMapL = 16;
	CHexMap HexMap[HexMapL] =
	{
		{'0', 0}, {'1', 1},
		{'2', 2}, {'3', 3},
		{'4', 4}, {'5', 5},
		{'6', 6}, {'7', 7},
		{'8', 8}, {'9', 9},
		{'A', 10}, {'B', 11},
		{'C', 12}, {'D', 13},
		{'E', 14}, {'F', 15}
	};
	QString mstr = Qvalue.toUpper();
	QString s = mstr;
	short result = 0;
	if (s.left(2) == "0X")
	{
		s = s.mid(2);
	}
	bool firsttime = true;
	for (int j = 0; j < s.length(); j++)
		//while (*s != '\0')
	{
		bool found = false;
		for (int i = 0; i < HexMapL; i++)
		{
			if (s.mid(j,1) == HexMap[i].chr)
			{
				if (!firsttime) result <<= 4;
				result |= HexMap[i].value;
				found = true;
				break;
			}
		}
		if (!found) break;
		//s++;
		firsttime = false;
	}
	//free(mstr);
	return result;
}


//////////////////////////////////////////////////////////////////////////ANALYSE AND TEST//////////////////////////////////////////////////////////////////////////


QString ReadBIOSLPTTable()
{

	unsigned int far *ptraddr;  /* Pointer to location of Port Addresses */
	unsigned int address;       /* Address of Port */
	int a;

	QString LPTInfo;

	ptraddr=(unsigned int far *)0x00000408;

	for (a = 0; a < 3; a++)
	{
		address = *ptraddr;
		if (address == 0)
			LPTInfo = LPTInfo + ("No port found for LPT%d \n",a+1);
		else
			LPTInfo = LPTInfo + ("Address assigned to LPT%d is %Xh\n",a+1,address);
		*ptraddr++;
	}
	return LPTInfo;
}

void PPort_Dialog::on_btnReadBIOS_clicked()
{
	QMessageBox msgBox(QMessageBox::Information,"Info","Not implemented yet...",QMessageBox::Ok,this);
	msgBox.exec();
}

void PPort_Dialog::on_okButton_clicked()
{
	cleanUp();
	accept();
}

void PPort_Dialog::on_cancelButton_clicked()
{
	cleanUp();
	reject();
}

void PPort_Dialog::cleanUp()
{
	return;
}

void PPort_Dialog::ReadPortValues()
{
	getATBaseAddress();
	//short baseAddress = _httoi(ui.txtATBaseAddress->text());

	//Data Port
	ui.chkDataPortB0->setChecked(UI_InpOutObject.ReadPortBit(nATBaseAddress,0));
	ui.chkDataPortB1->setChecked(UI_InpOutObject.ReadPortBit(nATBaseAddress,1));
	ui.chkDataPortB2->setChecked(UI_InpOutObject.ReadPortBit(nATBaseAddress,2));
	ui.chkDataPortB3->setChecked(UI_InpOutObject.ReadPortBit(nATBaseAddress,3));
	ui.chkDataPortB4->setChecked(UI_InpOutObject.ReadPortBit(nATBaseAddress,4));
	ui.chkDataPortB5->setChecked(UI_InpOutObject.ReadPortBit(nATBaseAddress,5));
	ui.chkDataPortB6->setChecked(UI_InpOutObject.ReadPortBit(nATBaseAddress,6));
	ui.chkDataPortB7->setChecked(UI_InpOutObject.ReadPortBit(nATBaseAddress,7));
	ui.txtDataPortReadValue->setText(QString::number(UI_InpOutObject.ReadPort(nATBaseAddress)));

	//Status Port
	ui.chkStatusPortB3->setChecked(UI_InpOutObject.ReadPortBit(nATBaseAddress + 1,3));
	ui.chkStatusPortB4->setChecked(UI_InpOutObject.ReadPortBit(nATBaseAddress + 1,4));
	ui.chkStatusPortB5->setChecked(UI_InpOutObject.ReadPortBit(nATBaseAddress + 1,5));
	ui.chkStatusPortB6->setChecked(UI_InpOutObject.ReadPortBit(nATBaseAddress + 1,6));
	ui.chkStatusPortB7->setChecked(UI_InpOutObject.ReadPortBit(nATBaseAddress + 1,7));
	ui.txtStatusPortReadValue->setText(QString::number(UI_InpOutObject.ReadPort(nATBaseAddress + 1)));

	//Control Port
	ui.chkControlPortB0->setChecked(UI_InpOutObject.ReadPortBit(nATBaseAddress + 2,0));
	ui.chkControlPortB1->setChecked(UI_InpOutObject.ReadPortBit(nATBaseAddress + 2,1));
	ui.chkControlPortB2->setChecked(UI_InpOutObject.ReadPortBit(nATBaseAddress + 2,2));
	ui.chkControlPortB3->setChecked(UI_InpOutObject.ReadPortBit(nATBaseAddress + 2,3));
	ui.chkControlPortB4->setChecked(UI_InpOutObject.ReadPortBit(nATBaseAddress + 2,4));
	ui.chkControlPortB5->setChecked(UI_InpOutObject.ReadPortBit(nATBaseAddress + 2,5));
	ui.txtControlPortReadValue->setText(QString::number(UI_InpOutObject.ReadPort(nATBaseAddress + 2)));

}

void PPort_Dialog::WritePortValues()
{
	getATBaseAddress();
	//short baseAddress = _httoi(ui.txtATBaseAddress->text());

	//Data Port		
	UI_InpOutObject.WritePortBit(nATBaseAddress,0,ui.chkDataPortB0->checkState());
	UI_InpOutObject.WritePortBit(nATBaseAddress,1,ui.chkDataPortB1->checkState());
	UI_InpOutObject.WritePortBit(nATBaseAddress,2,ui.chkDataPortB2->checkState());
	UI_InpOutObject.WritePortBit(nATBaseAddress,3,ui.chkDataPortB3->checkState());
	UI_InpOutObject.WritePortBit(nATBaseAddress,4,ui.chkDataPortB4->checkState());
	UI_InpOutObject.WritePortBit(nATBaseAddress,5,ui.chkDataPortB5->checkState());
	UI_InpOutObject.WritePortBit(nATBaseAddress,6,ui.chkDataPortB6->checkState());
	UI_InpOutObject.WritePortBit(nATBaseAddress,7,ui.chkDataPortB7->checkState());

	//Status Port		
	UI_InpOutObject.WritePortBit(nATBaseAddress + 1,3,ui.chkStatusPortB3->checkState());
	UI_InpOutObject.WritePortBit(nATBaseAddress + 1,4,ui.chkStatusPortB4->checkState());
	UI_InpOutObject.WritePortBit(nATBaseAddress + 1,5,ui.chkStatusPortB5->checkState());
	UI_InpOutObject.WritePortBit(nATBaseAddress + 1,6,ui.chkStatusPortB6->checkState());
	UI_InpOutObject.WritePortBit(nATBaseAddress + 1,7,ui.chkStatusPortB7->checkState());

	//Control Port		
	UI_InpOutObject.WritePortBit(nATBaseAddress + 2,0,ui.chkControlPortB0->checkState());
	UI_InpOutObject.WritePortBit(nATBaseAddress + 2,1,ui.chkControlPortB1->checkState());
	UI_InpOutObject.WritePortBit(nATBaseAddress + 2,2,ui.chkControlPortB2->checkState());
	UI_InpOutObject.WritePortBit(nATBaseAddress + 2,3,ui.chkControlPortB3->checkState());
	UI_InpOutObject.WritePortBit(nATBaseAddress + 2,4,ui.chkControlPortB4->checkState());
	UI_InpOutObject.WritePortBit(nATBaseAddress + 2,5,ui.chkControlPortB5->checkState());
}

void PPort_Dialog::GetCurrentPortMode(QString message, short PortAddr)
{
	QMessageBox msgBox(QMessageBox::Information,"LPT Port Mode",message + UI_InpOutObject.GetPortType(PortAddr) + ": " + UI_InpOutObject.GetPortDescription(PortAddr),QMessageBox::Ok,this);
	msgBox.exec();
}

void PPort_Dialog::on_btnSetInputMode_clicked()
{
	//Set Input Mode
	getATBaseAddress();
	UI_InpOutObject.ConfigurePortForInput(nATBaseAddress);
	ReadPortValues();
	GetCurrentPortMode("The Port is set to: ",nATBaseAddress);
}

void PPort_Dialog::on_btnSetOutputMode_clicked()
{
	//Set Output Mode
	getATBaseAddress();
	UI_InpOutObject.ConfigurePortForOutput(nATBaseAddress);
	ReadPortValues();
	GetCurrentPortMode("The Port is set to: ",nATBaseAddress);
}

void PPort_Dialog::on_btnGetMode_clicked()
{
	//Get Current Mode
	ReadPortValues();
	GetCurrentPortMode("The current Mode is: ",nATBaseAddress);
}

void PPort_Dialog::on_btnReadPort_clicked()
{
	ReadPortValues();
}

void PPort_Dialog::on_btnWritePort_clicked()
{
	//Write Port, Scan all three registers and update the display.
	WritePortValues();
	ReadPortValues();
}

short PPort_Dialog::getATBaseAddress()
{
	if (ui.rbBaseAddrinHex->isChecked())
	{
		nATBaseAddress = _httoi(ui.txtATBaseAddress->text());
	}
	else
	{
		nATBaseAddress = (ui.txtATBaseAddress->text().toShort());
	}	
	return nATBaseAddress;
}


//////////////////////////////////////////////////////////////////////////CAPTURE//////////////////////////////////////////////////////////////////////////

short PPort_Dialog::getCBaseAddress()
{
	if (ui.rbBaseAddrinHex_C->isChecked())
	{
		nCBaseAddress = _httoi(ui.txtCBaseAddress->text());
	}
	else
	{
		nCBaseAddress = (ui.txtCBaseAddress->text().toShort());
	}	
	return nCBaseAddress;
}

void PPort_Dialog::UpdateCaptureThreadUI(int Value)
{
	ui.txtFeedBackTriggVal->setText(QString::number(Value));
	ui.txt_NrOfFeedBacks->setText(QString::number((ui.txt_NrOfFeedBacks->text().toInt()+1)));
}

void PPort_Dialog::CaptureThreadStarted(QString dateTimeStampText)
{
	captureThreadRunning = true;
	ui.rdb_CaptureThreadRunning->setChecked(true);
	ui.gb_CaptureThread->setEnabled(false);
	ui.txt_NrOfFeedBacks->setText("0");
	ui.txtFeedBackTriggVal->setText("");
	ui.btnStartCaptureThread->setEnabled(false);
	ui.txtFeedBackStopped->setText("");
	ui.txtFeedBackStarted->setText(dateTimeStampText);
}

void PPort_Dialog::CaptureThreadStopped(QString dateTimeStampText)
{
	ui.rdb_CaptureThreadRunning->setChecked(false);
	ui.gb_CaptureThread->setEnabled(true);
	ui.btnStartCaptureThread->setEnabled(true);
	ui.txtFeedBackStopped->setText(dateTimeStampText);
	captureThreadRunning = false;
}

short PPort_Dialog::getCBitMask()
{
	short tmpMask = 0;
	if (ui.chkCaptureMaskB0->isChecked())  tmpMask = tmpMask + 1;
	if (ui.chkCaptureMaskB1->isChecked()) tmpMask = tmpMask + 2;
	if (ui.chkCaptureMaskB2->isChecked()) tmpMask = tmpMask + 4;
	if (ui.chkCaptureMaskB3->isChecked()) tmpMask = tmpMask + 8;
	if (ui.chkCaptureMaskB4->isChecked()) tmpMask = tmpMask + 16;
	if (ui.chkCaptureMaskB5->isChecked()) tmpMask = tmpMask + 32;
	if (ui.chkCaptureMaskB6->isChecked()) tmpMask = tmpMask + 64;
	if (ui.chkCaptureMaskB7->isChecked()) tmpMask = tmpMask + 128;
	return tmpMask;
}

int PPort_Dialog::getCPostLHTiming()
{
	return ui.txtLHDelay_C->text().toInt();
}

int PPort_Dialog::getCPostHLTiming()
{
	return ui.txtHLDelay_C->text().toInt();
}

ParallelPortNameSpace::CaptureMethod PPort_Dialog::getCCaptureMethod()
{
	//short tmpMask = 0;
    if (ui.rbDecMethod_MVC0->isChecked())  return ParallelPortNameSpace::MaskedValueChanged;
    if (ui.rbDecMethodHigh_MVC1->isChecked())  return ParallelPortNameSpace::MaskedValueChangedHigh;
    if (ui.rbDecMethodLow_MVC2->isChecked())  return ParallelPortNameSpace::MaskedValueChangedLow;
    return ParallelPortNameSpace::MaskedValueChanged;
}

void PPort_Dialog::on_btnStartCaptureThread_clicked()
{
	QMessageBox::StandardButton ret;	
	if (generateThreadRunning)
	{
		ret = QMessageBox::warning(this, tr("Another thread Running!"),
			tr("Another thread is already running, this may cause some unexpected behavior.\n"
			"Do you want to stop the other thread first?"),
			QMessageBox::Yes | QMessageBox::No);
		if (ret == QMessageBox::Yes)
		{
			generateThread->stop();
		}
	}
	if ((UI_InpOutObject.IsPortInput(getCBaseAddress()))==false)
	{
		ret = QMessageBox::warning(this, tr("Try to change to Input Mode?"),
			tr("The current port is NOT configured for input, if you are trying to read from the Data-port this port first needs to be set to input mode.\n"
			"Do you want to try this first before aborting the capture?"),
			QMessageBox::Yes | QMessageBox::No);
		if (ret == QMessageBox::Yes)
		{
			UI_InpOutObject.ConfigurePortForInput(nCBaseAddress);
			if ((UI_InpOutObject.IsPortInput(getCBaseAddress()))==false)
			{
				ret = QMessageBox::warning(this, tr("Failed to configure the Input Mode!"),
					tr("The mode could not be changed to Input Mode. Maybe your hardware doesn't support it?\n"
					"Do you still want to proceed?"),
					QMessageBox::Yes | QMessageBox::No);
				if (ret == QMessageBox::No)
				{
					return;
				}
			}
		}
		else
		{
			return;
		}
	}
	captureThread = new ppCaptureThread(nCBaseAddress,getCBitMask(),getCCaptureMethod(),getCPostLHTiming(),getCPostHLTiming(),this);
	connect(captureThread, SIGNAL(recieveThreadTriggered(int)), this, SLOT(UpdateCaptureThreadUI(int)));
	connect(captureThread, SIGNAL(recieveThreadStarted(QString)), this, SLOT(CaptureThreadStarted(QString)));
	connect(captureThread, SIGNAL(recieveThreadStopped(QString)), this, SLOT(CaptureThreadStopped(QString)));
	captureThread->start();
	//emit recieveThreadStarted(QDateTime::currentDateTime().toString(Qt::TextDate));
	//captureThread->wait();//blocks current event loop
	//QMessageBox msgBox(QMessageBox::Information,"Test","Test",QMessageBox::Ok,this);
	//msgBox.exec();
}

void PPort_Dialog::on_btnStopCaptureThread_clicked()
{
	captureThread->stop();
}

void PPort_Dialog::on_rbDecMethod_MVC0_toggled(bool)
{
	if (ui.rbDecMethod_MVC0->isChecked())
	{
		ui.txtLHDelay_C->setEnabled(true);
		ui.txtHLDelay_C->setEnabled(true);
	}
}

void PPort_Dialog::on_rbDecMethodHigh_MVC1_toggled(bool)
{
	if (ui.rbDecMethodHigh_MVC1->isChecked())
	{
		ui.txtLHDelay_C->setEnabled(true);
		ui.txtHLDelay_C->setEnabled(false);
	}
}

void PPort_Dialog::on_rbDecMethodLow_MVC2_toggled(bool)
{
	if (ui.rbDecMethodLow_MVC2->isChecked())
	{
		ui.txtLHDelay_C->setEnabled(false);
		ui.txtHLDelay_C->setEnabled(true);
	}
}



//////////////////////////////////////////////////////////////////////////GENERATE//////////////////////////////////////////////////////////////////////////





void PPort_Dialog::on_rbGenMethod_Value_toggled(bool)
{
	if (ui.rbGenMethod_Value->isChecked()) 
	{
		ui.gb_GenInActVal->setEnabled(false);
		ui.gb_GenPulseShape->setEnabled(false);
		ui.txtRepetitionTime_G->setEnabled(false);
	}
}

void PPort_Dialog::on_rbGenMethod_Pulse_toggled(bool)
{
	if (ui.rbGenMethod_Pulse->isChecked()) 
	{
		ui.gb_GenInActVal->setEnabled(true);
		ui.gb_GenPulseShape->setEnabled(true);	
		ui.txtRepetitionTime_G->setEnabled(false);
	}
}

void PPort_Dialog::on_rbGenMethod_Periodical_toggled(bool)
{
	if (ui.rbGenMethod_Periodical->isChecked())	
	{
		ui.gb_GenInActVal->setEnabled(true);
		ui.gb_GenPulseShape->setEnabled(true);
		ui.txtRepetitionTime_G->setEnabled(true);
	}
}

void PPort_Dialog::on_chkGenerateMaskB0_stateChanged(int)
{
	ui.chkGenerateActiveB0->setEnabled(ui.chkGenerateMaskB0->isChecked());
	ui.chkGenerateInActiveB0->setEnabled(ui.chkGenerateMaskB0->isChecked());
}

void PPort_Dialog::on_chkGenerateMaskB1_stateChanged(int)
{
	ui.chkGenerateActiveB1->setEnabled(ui.chkGenerateMaskB1->isChecked());
	ui.chkGenerateInActiveB1->setEnabled(ui.chkGenerateMaskB1->isChecked());
}

void PPort_Dialog::on_chkGenerateMaskB2_stateChanged(int)
{
	ui.chkGenerateActiveB2->setEnabled(ui.chkGenerateMaskB2->isChecked());
	ui.chkGenerateInActiveB2->setEnabled(ui.chkGenerateMaskB2->isChecked());
}

void PPort_Dialog::on_chkGenerateMaskB3_stateChanged(int)
{
	ui.chkGenerateActiveB3->setEnabled(ui.chkGenerateMaskB3->isChecked());
	ui.chkGenerateInActiveB3->setEnabled(ui.chkGenerateMaskB3->isChecked());
}

void PPort_Dialog::on_chkGenerateMaskB4_stateChanged(int)
{
	ui.chkGenerateActiveB4->setEnabled(ui.chkGenerateMaskB4->isChecked());
	ui.chkGenerateInActiveB4->setEnabled(ui.chkGenerateMaskB4->isChecked());
}

void PPort_Dialog::on_chkGenerateMaskB5_stateChanged(int)
{
	ui.chkGenerateActiveB5->setEnabled(ui.chkGenerateMaskB5->isChecked());
	ui.chkGenerateInActiveB5->setEnabled(ui.chkGenerateMaskB5->isChecked());
}

void PPort_Dialog::on_chkGenerateMaskB6_stateChanged(int)
{
	ui.chkGenerateActiveB6->setEnabled(ui.chkGenerateMaskB6->isChecked());
	ui.chkGenerateInActiveB6->setEnabled(ui.chkGenerateMaskB6->isChecked());
}

void PPort_Dialog::on_chkGenerateMaskB7_stateChanged(int)
{
	ui.chkGenerateActiveB7->setEnabled(ui.chkGenerateMaskB7->isChecked());
	ui.chkGenerateInActiveB7->setEnabled(ui.chkGenerateMaskB7->isChecked());
}

short PPort_Dialog::getGBaseAddress()
{
	if (ui.rbBaseAddrinHex_G->isChecked())
	{
		nGBaseAddress = _httoi(ui.txtGBaseAddress->text());
	}
	else
	{
		nGBaseAddress = (ui.txtGBaseAddress->text().toShort());
	}	
	return nGBaseAddress;
}

void PPort_Dialog::UpdateGenerateThreadUI(int Value)
{
	ui.txtFeedBackGenVal->setText(QString::number(Value));
	ui.txt_GenNrOfFeedBacks->setText(QString::number((ui.txt_GenNrOfFeedBacks->text().toInt()+1)));
}

void PPort_Dialog::GenerateThreadStarted(QString dateTimeStampText)
{
	generateThreadRunning = true;
	ui.rdb_GenerateThreadRunning->setChecked(true);
	ui.gb_GenerateThread->setEnabled(false);
	ui.txt_GenNrOfFeedBacks->setText("0");
	ui.txtFeedBackGenVal->setText("");
	ui.btnStartGenerateThread->setEnabled(false);
	ui.txtGenFeedBackStopped->setText("");
	ui.txtGenFeedBackStarted->setText(dateTimeStampText);
}

void PPort_Dialog::GenerateThreadStopped(QString dateTimeStampText)
{
	ui.rdb_GenerateThreadRunning->setChecked(false);
	ui.gb_GenerateThread->setEnabled(true);
	ui.btnStartGenerateThread->setEnabled(true);
	ui.txtGenFeedBackStopped->setText(dateTimeStampText);
	generateThreadRunning = false;
}

short PPort_Dialog::getGBitMaskValue()
{
	short tmpMask = 0;
	if ((ui.chkGenerateMaskB0->isChecked()) & (ui.chkGenerateMaskB0->isEnabled()))  tmpMask = tmpMask + 1;
	if ((ui.chkGenerateMaskB1->isChecked()) & (ui.chkGenerateMaskB1->isEnabled()))  tmpMask = tmpMask + 2;
	if ((ui.chkGenerateMaskB2->isChecked()) & (ui.chkGenerateMaskB2->isEnabled()))  tmpMask = tmpMask + 4;
	if ((ui.chkGenerateMaskB3->isChecked()) & (ui.chkGenerateMaskB3->isEnabled()))  tmpMask = tmpMask + 8;
	if ((ui.chkGenerateMaskB4->isChecked()) & (ui.chkGenerateMaskB4->isEnabled()))  tmpMask = tmpMask + 16;
	if ((ui.chkGenerateMaskB5->isChecked()) & (ui.chkGenerateMaskB5->isEnabled()))  tmpMask = tmpMask + 32;
	if ((ui.chkGenerateMaskB6->isChecked()) & (ui.chkGenerateMaskB6->isEnabled()))  tmpMask = tmpMask + 64;
	if ((ui.chkGenerateMaskB7->isChecked()) & (ui.chkGenerateMaskB7->isEnabled()))  tmpMask = tmpMask + 128;
	return tmpMask;
}

short PPort_Dialog::getGBitActiveValue()
{
	short tmpMask = 0;
	if ((ui.chkGenerateActiveB0->isChecked()) & (ui.chkGenerateActiveB0->isEnabled()))  tmpMask = tmpMask + 1;
	if ((ui.chkGenerateActiveB1->isChecked()) & (ui.chkGenerateActiveB1->isEnabled()))  tmpMask = tmpMask + 2;
	if ((ui.chkGenerateActiveB2->isChecked()) & (ui.chkGenerateActiveB2->isEnabled()))  tmpMask = tmpMask + 4;
	if ((ui.chkGenerateActiveB3->isChecked()) & (ui.chkGenerateActiveB3->isEnabled()))  tmpMask = tmpMask + 8;
	if ((ui.chkGenerateActiveB4->isChecked()) & (ui.chkGenerateActiveB4->isEnabled()))  tmpMask = tmpMask + 16;
	if ((ui.chkGenerateActiveB5->isChecked()) & (ui.chkGenerateActiveB5->isEnabled()))  tmpMask = tmpMask + 32;
	if ((ui.chkGenerateActiveB6->isChecked()) & (ui.chkGenerateActiveB6->isEnabled()))  tmpMask = tmpMask + 64;
	if ((ui.chkGenerateActiveB7->isChecked()) & (ui.chkGenerateActiveB7->isEnabled()))  tmpMask = tmpMask + 128;
	return tmpMask;
}

short PPort_Dialog::getGBitInActiveValue()
{
	short tmpMask = 0;
	if ((ui.chkGenerateInActiveB0->isChecked()) & (ui.chkGenerateInActiveB0->isEnabled()))  tmpMask = tmpMask + 1;
	if ((ui.chkGenerateInActiveB1->isChecked()) & (ui.chkGenerateInActiveB1->isEnabled()))  tmpMask = tmpMask + 2;
	if ((ui.chkGenerateInActiveB2->isChecked()) & (ui.chkGenerateInActiveB2->isEnabled()))  tmpMask = tmpMask + 4;
	if ((ui.chkGenerateInActiveB3->isChecked()) & (ui.chkGenerateInActiveB3->isEnabled()))  tmpMask = tmpMask + 8;
	if ((ui.chkGenerateInActiveB4->isChecked()) & (ui.chkGenerateInActiveB4->isEnabled()))  tmpMask = tmpMask + 16;
	if ((ui.chkGenerateInActiveB5->isChecked()) & (ui.chkGenerateInActiveB5->isEnabled()))  tmpMask = tmpMask + 32;
	if ((ui.chkGenerateInActiveB6->isChecked()) & (ui.chkGenerateInActiveB6->isEnabled()))  tmpMask = tmpMask + 64;
	if ((ui.chkGenerateInActiveB7->isChecked()) & (ui.chkGenerateInActiveB7->isEnabled()))  tmpMask = tmpMask + 128;
	return tmpMask;
}

int PPort_Dialog::getGActivePulseTime()
{
	return ui.txtPulseActiveTime_G->text().toInt();
}

int PPort_Dialog::getGRepetitionTime()
{
	return ui.txtRepetitionTime_G->text().toInt();
}

ParallelPortNameSpace::GenerationMethod PPort_Dialog::getGGenerationMethod()
{
    if (ui.rbGenMethod_Value->isChecked())  return ParallelPortNameSpace::Value;
    if (ui.rbGenMethod_Pulse->isChecked())  return ParallelPortNameSpace::Pulse;
    if (ui.rbGenMethod_Periodical->isChecked())  return ParallelPortNameSpace::Periodical;
    return ParallelPortNameSpace::Value;
}

void PPort_Dialog::on_btnStopGenerateThread_clicked()
{
	generateThread->stop();
}

void PPort_Dialog::on_btnStartGenerateThread_clicked()
{
	QMessageBox::StandardButton ret;	
	if (captureThreadRunning)
	{
		ret = QMessageBox::warning(this, tr("Another thread Running!"),
			tr("Another thread is already running, this may cause some unexpected behavior.\n"
			"Do you want to stop the other thread first?"),
			QMessageBox::Yes | QMessageBox::No);
		if (ret == QMessageBox::Yes)
		{
			captureThread->stop();
		}
	}
	if ((UI_InpOutObject.IsPortOutput(getGBaseAddress()))==false)
	{
		ret = QMessageBox::warning(this, tr("Change to Output Mode?"),
			tr("The current port is not configured for output, to proceed the port needs to be set to output mode.\n"
			"Do you want to proceed?"),
			QMessageBox::Yes | QMessageBox::No);
		if (ret == QMessageBox::Yes)
		{
			UI_InpOutObject.ConfigurePortForOutput(nGBaseAddress);
		}
		else
		{
			return;
		}
	}
	generateThread = new ppGenerateThread(nGBaseAddress,(int)getGGenerationMethod(),getGBitMaskValue(),getGBitActiveValue(),getGBitInActiveValue(),getGActivePulseTime(),getGRepetitionTime(),this);
	connect(generateThread, SIGNAL(generateThreadTriggered(int)), this, SLOT(UpdateGenerateThreadUI(int)));
	connect(generateThread, SIGNAL(generateThreadStarted(QString)), this, SLOT(GenerateThreadStarted(QString)));
	connect(generateThread, SIGNAL(generateThreadStopped(QString)), this, SLOT(GenerateThreadStopped(QString)));	
	generateThread->start();
}
