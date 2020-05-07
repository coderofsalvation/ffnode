/*********************************************************************************

    FFmpegGUI: filter graph editor based on Qt and FFmpeg
    Copyright (C) 2017 Roman Sichkaruk <romansichkaruk@gmail.com>

    This file is part of FFmpegGUI

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

***********************************************************************************/

#include <QApplication>
#include <QPalette>
#include <iostream>
#include "player.h"
#include "mainwindow.h"


int main(int argc, char *argv[]) {
    // initialize resources, if needed
    // Q_INIT_RESOURCE(resfile);

    QApplication app(argc, argv);

	app.setStyle("Fusion");
	QPalette p;
	p = app.palette();
	p.setColor(QPalette::Window, QColor(43,43,53));
	p.setColor(QPalette::Button, QColor(43,43,53));
	p.setColor(QPalette::Base, QColor(43,43,53));
	p.setColor(QPalette::AlternateBase, QColor(43,43,53));
	p.setColor(QPalette::Highlight, QColor(142,45,197));
	p.setColor(QPalette::ButtonText, QColor(255,255,255));
	p.setColor(QPalette::ToolTipBase, Qt::white);
	p.setColor(QPalette::ToolTipText, QColor(43,43,53));
	p.setColor(QPalette::Text, Qt::white);
	p.setColor(QPalette::WindowText, QColor(140,140,140));
	p.setColor(QPalette::BrightText, QColor(255,183,22) );
	p.setColor(QPalette::Link, QColor(255,183,22) );
	p.setColor(QPalette::Highlight, QColor(255,183,22));
	app.setPalette(p);
	
	app.setStyleSheet("QTableView{ border: none; }");
    
    MainWindow w;
    w.show();
    return app.exec();
}
