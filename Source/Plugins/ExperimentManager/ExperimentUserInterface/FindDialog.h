//ExperimentManagerplugin
//Copyright (C) 2014  Sven Gijsen
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

#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QWidget>

namespace Ui
{
    class FindDialog;
}

class FindDialog : public QWidget
{
    Q_OBJECT

    signals:
        void findSignal(const QString &text, const QStringList &filters);
        
    public:
        explicit FindDialog(QWidget *parent = 0);
        ~FindDialog();
        
    private:
        Ui::FindDialog *ui;

    private slots:
        void findSlot();
};

#endif // FINDDIALOG_H
