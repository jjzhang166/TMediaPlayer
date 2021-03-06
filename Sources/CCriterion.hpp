/*
Copyright (C) 2012-2016 Teddy Michel

This file is part of TMediaPlayer.

TMediaPlayer is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

TMediaPlayer is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with TMediaPlayer. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef FILE_C_CRITERIA
#define FILE_C_CRITERIA

#include "ICriterion.hpp"


/**
 * Critère simple.
 */

class CCriterion : public ICriterion
{
    Q_OBJECT

public:

    explicit CCriterion(CMainWindow * mainWindow, QObject * parent = nullptr);
    virtual ~CCriterion();

    virtual bool matchCriterion(CSong * song) const;
    virtual QList<CSong *> getSongs(const QList<CSong *>& from, const QList<CSong *>& with = QList<CSong *>(), bool onlyChecked = false) const;
    virtual TUpdateConditions getUpdateConditions() const;
    virtual IWidgetCriterion * getWidget() const;
};

#endif // FILE_C_CRITERIA
