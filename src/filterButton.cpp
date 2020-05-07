
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


#include "filterButton.h"
#include "scene.h"
#include <stdio.h>

/**
 *  Constructors and destructors
 */
FilterButton::FilterButton(const QString &name) {
    
    this->setProperty("name", name);
    this->setGeometry(QRect(0, 0, 200,90));
    this->setFixedWidth(200);
    this->setIconSize(QSize(200,90));
    this->setIcon(createPixmap());
    this->setFixedSize(200,40);
   	this->setFlat(true);
}

FilterButton::FilterButton(const FilterButton& orig) {}
FilterButton::~FilterButton() {}
//------------------------------------------------------------------------------

/**
 * This function creates QPixmap image representing filter
 * @return 
 */

QPixmap FilterButton::createPixmap() {
	int w = 170;
	int h = 50;
    QPixmap p(":/filt1.png");
    QPixmap finalIcon = p.scaled(w,h);
    
    QPainter painter(&finalIcon);
    QFont font = painter.font();
    //font.setWeight(QFont::Bold);
    font.setPointSize ( 9 );
	painter.setPen( QColor(200,200,200) );
    painter.setFont(font); 

    QRect r(0, 0, w,h);
    painter.drawText(r, Qt::AlignCenter | Qt::TextWrapAnywhere, 
                     property("name").toString());
    QSize size;
    
    if(property("name").toString().compare("input") == 0)
        size = QSize(0,1);
    else if(property("name").toString().compare("output") == 0)
        size = QSize(1,0);
    else
        size = Scene::filtersSize[property("name").toString()];
    
    drawPixmap(&painter, size);
    
    return finalIcon;
}
//------------------------------------------------------------------------------

/**
 * This function draws filter pads' icons on image that
 * represents filter.
 * @param painter is reference to QPainter object that provides painting
 * @param size is QSize containing count of input and output pads
 */

void FilterButton::drawPixmap(QPainter * painter, const QSize & size){
    
    if(size.width() == 1){
        painter->drawImage( QRect(1,9,18,18), 
                            QPixmap(":/pad1.png").scaled(13,13).toImage(),
                            QRect(0,0,18,18));
    }
    else if(size.width() == 2){
        painter->drawImage(  QRect(0,2,18,18), 
                            QPixmap(":/pad1.png").scaled(13,13).toImage(),
                            QRect(0,0,18,18));
        painter->drawImage(  QRect(0,30,18,18), 
                            QPixmap(":/pad1.png").scaled(13,13).toImage(),
                            QRect(0,0,18,18));
    }
    
    if(size.height() == 1){
        painter->drawImage(  QRect(182,16,18,18), 
                            QPixmap(":/pad1.png").scaled(13,13).toImage(),
                            QRect(0,0,18,18)); 
    }
    else if(size.height() == 2){
        painter->drawImage(  QRect(182,2,18,18), 
                            QPixmap(":/pad1.png").scaled(13,11).toImage(),
                            QRect(0,0,18,18));
        painter->drawImage( QRect(182,30,18,18), 
                            QPixmap(":/pad1.png").scaled(13,13).toImage(),
                            QRect(0,0,18,18));
    }
}
//------------------------------------------------------------------------------

/**
 * This function handles mouse press event on FilterButton
 * instances. Creates new QDrag and sets its QMimeData to 
 * FilterButton instance's name, so it can be used to reconstruct 
 * in Drop event handler.
 * @param e mouse event to be handled
 */

void FilterButton::mousePressEvent(QMouseEvent *e){
    
    QDrag *drag = new QDrag(this);
    QMimeData *mime = new QMimeData;
    drag->setMimeData(mime);
    mime->setText(this->property("name").toString());

    
    drag->setPixmap(createPixmap());
    drag->setHotSpot(QPoint(100, 25));

    drag->exec();
}
//------------------------------------------------------------------------------
