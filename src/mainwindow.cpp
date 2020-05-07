
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

#include "mainwindow.h"
#include "filtersList.h"
#include "pipelineParser.h"
#include "pipelineSaver.h"
#include "default.h"
#include <stdio.h>

/**
 * Construrs and destructor
 */

MainWindow::MainWindow() {
    
    av_register_all();
    avcodec_register_all();
    avfilter_register_all();
    
    initializeHelp();
    createActions();
    actualPipeline=NULL;
    
    prepareScene();
    createFilterBar();
  
    filterList = new FilterParamsList();
    this->addDockWidget(Qt::RightDockWidgetArea, filterList);
    this->scene->setFilterParamList(filterList);
    
    filterBar = new FiltersList();
	filterBar->hide();
    this->addDockWidget(Qt::LeftDockWidgetArea, filterBar);
    
    fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(clear);
	QMenu * submenu = fileMenu->addMenu(tr("&Open"));
    submenu->addAction(newAct);
	submenu = fileMenu->addMenu(tr("&Save"));
	submenu->addAction(save);
    submenu->addAction(saveXml);
    submenu->addAction(saveTxt);
    fileMenu->addAction(exitAll);
    fileMenu = menuBar()->addMenu(tr("&View"));
    fileMenu->addAction(showFList);
    fileMenu->addAction(showPList);
    fileMenu = menuBar()->addMenu(tr("&Exit"));
    fileMenu->addAction(exitAll);
    
	this->clearAllDefault();
    this->show();
    insWidget = new InspectionWidget();
    this->addDockWidget(Qt::RightDockWidgetArea, insWidget);
}

MainWindow::MainWindow(const MainWindow& orig) {}

MainWindow::~MainWindow() {
    
    delete unicorn;
    delete helpText;
    delete scene;
    delete view;
    delete insWidget;
    delete c;
    delete fileToolBar;
    
    delete newAct;
    delete saveXml;
    delete saveTxt;
    delete wire;
    delete save;
    delete curs;
    delete clear;
    delete remove;
    delete hand;
    delete play;
    delete text;
    delete help;
    delete automatic;
    delete exitAll;
    delete showFList;
    delete showPList;
    
    delete fileMenu;
    delete filterList;
    delete shower;
}
//------------------------------------------------------------------------------

/**
 * Initializes help labels.
 */

void MainWindow::initializeHelp(){
    unicorn = new QLabel(this);
    unicorn->setPixmap(QPixmap(":/unicorn.png"));
    unicorn->setScaledContents( true ); 
    unicorn->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
 
    helpText = new QLabel(this);
    helpText->setPixmap(QPixmap(":/buble.png"));
    helpText->setScaledContents( true ); 
    helpText->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
}
//------------------------------------------------------------------------------

/**
 * Creates scene and initializes its size.
 */

void MainWindow::prepareScene(){
    c = new Connectivity();
    this->scene = new Scene(c);
	this->scene->setMainWindow(this);
    this->scene->setSceneRect(0.0, 0.0,1500.0,1500.0);
	this->scene->setBackgroundBrush(QColor(25, 22, 20));
    this->acceptDrops(); 
    this->view = new View();
    this->view->setScene(this->scene);
    setCentralWidget(view);
}
//------------------------------------------------------------------------------
    
/**
 * Shows/hides parameters.
 */

void MainWindow::showP(){
    if(filterList->isVisible()){
        filterList->hide();
        showPList->setIcon(QIcon(":/notcheck.png"));
    }
    else{
        filterList->show();
        showPList->setIcon(QIcon(":/checked.png"));
    }
    
}
//------------------------------------------------------------------------------

/**
 * Shows/hides filters.
 */

void MainWindow::showF(){
    if(filterBar->isVisible()){
        filterBar->hide();
		fileToolBar->hide();
        showFList->setIcon(QIcon(":/notcheck.png"));
    }
    else{
        filterBar->show();
		fileToolBar->show();
        showFList->setIcon(QIcon(":/checked.png"));
    }
}
//------------------------------------------------------------------------------

/**
 * Returns name of an ouput file.
 * @return 
 */

QString MainWindow::outputName(){
    for(auto f : scene->filters)
        if(f->getName() == "output") 
            return f->params[0]->value;
    
    return QString();
}
//------------------------------------------------------------------------------

/**
 * Exits from application
 */

void MainWindow::exitP(){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Exit", "Do you really want to quit?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) 
      QApplication::quit();
}
//------------------------------------------------------------------------------

/**
 * Setups QActions in a toolbar.
 */

void MainWindow::createActions()
{
    fileToolBar = addToolBar(tr("Quick actions"));
	fileToolBar->hide();
    
    newAct = createNewAction("Load graph.", ":/file.png", Qt::Key_G);
    connect(newAct, SIGNAL(triggered()), this, SLOT(addInput()));
     
    save = createNewAction("Save video.", ":/save.png", Qt::Key_S);
    connect(save, SIGNAL(triggered()), this, SLOT(saveVideo()));
    
    saveXml = createNewAction("Save graph to XML.", ":/xml.png", Qt::Key_E);
    connect(saveXml, SIGNAL(triggered()), this, SLOT(saveXML()));
    
    saveTxt = createNewAction("Save graph to txt file.", ":/savetext.png", Qt::Key_T);
    connect(saveTxt, SIGNAL(triggered()), this, SLOT(saveTXT()));
    
    remove = createNewAction("Delete element.",":/remove.png", Qt::Key_Delete);
    connect(remove, SIGNAL(triggered()), this, SLOT(removeItem()));
    
    clear = createNewAction("New",":/clear.png",Qt::Key_C);
    connect(clear, SIGNAL(triggered()), this, SLOT(clearAllDefault()));
    
    wire = createNewAction("Create a connection between two pads.", ":/wire.png", Qt::Key_W);
    connect(wire, SIGNAL(triggered()), this, SLOT(connectWire()));
    
    automatic = createNewAction("Automaticly connect filters.", ":/automatic.png", Qt::Key_A);
    connect(automatic, SIGNAL(triggered()), this, SLOT(autoConnect()));
    
    hand = createNewAction("Change cursor to hand.", ":/hand.png", Qt::Key_Q);
    connect(hand, SIGNAL(triggered()), this, SLOT(handTogled()));
    
    curs = createNewAction("Change cursor to arrow.", ":/cursor.png", Qt::Key_G);
    connect(curs, SIGNAL(triggered()), this, SLOT(cursTogled()));
    
    play = createNewAction("Play preview.", ":/play.png", Qt::Key_P);
    connect(play, SIGNAL(triggered()), this, SLOT(playVideo()));
    
    text = createNewAction("Show current graph string representation.",":/text.png", Qt::Key_T);
    connect(text, SIGNAL(triggered()), this, SLOT(showText()));
    
    help = createNewAction("Show help on inspected item.",":/help.png", Qt::Key_H);
    connect(help, SIGNAL(triggered()), this, SLOT(showHelp()));
    
    
    exitAll = new QAction("Exit program.", this);
    exitAll->setIcon(QIcon(":/exit.png"));
    //exitAll->setShortcut(Qt::Key_E);
    exitAll->setStatusTip(tr("Exit program."));
    connect(exitAll, SIGNAL(triggered()), this, SLOT(exitP()));
    
    showFList = new QAction("Show/hide filters list.", this);
    showFList->setIcon(QIcon(":/checked.png"));
    showFList->setShortcut(Qt::Key_Z);
    showFList->setStatusTip(tr("&Show/hide filters list."));
    connect(showFList, SIGNAL(triggered()), this, SLOT(showF()));
    
    showPList = new QAction("Show/hide parameters list.", this);
    showPList->setIcon(QIcon(":/checked.png"));
    showPList->setShortcut(Qt::Key_X);
    showPList->setStatusTip(tr("&Show/hide parameters list."));
    connect(showPList, SIGNAL(triggered()), this, SLOT(showP()));
}
//------------------------------------------------------------------------------

/**
 * Creates new QAction
 * @param info  - description of an action
 * @param icon  - icon of an action
 * @param key   - shortcut for an action
 * @return reference to action     
 */

QAction * MainWindow::createNewAction(const QString &info, const QString &icon, Qt::Key key){
    QAction * tmp = new QAction(info, this);
    tmp->setIcon(QIcon(icon));
    tmp->setShortcut(key);
    tmp->setStatusTip(info);
    fileToolBar->addAction(tmp);
    return tmp;
}
//------------------------------------------------------------------------------

/**
 *  Initializes widget with parameters table.
 */

void MainWindow::createFilterBar()
{
    AVFilter *avfil = NULL;
    while((avfil = avfilter_next(avfil))){
        int i = 0;
/*        
        bool onlyVideo=(avfilter_pad_count(avfil->inputs)>=1);
        for( ; i < avfilter_pad_count(avfil->inputs); i++){
            if(avfilter_pad_get_type(avfil->inputs, i) != AVMEDIA_TYPE_VIDEO)
                onlyVideo=false;
        }
        if(onlyVideo){
            onlyVideo=(avfilter_pad_count(avfil->outputs)>=1);
            for(i=0; i < avfilter_pad_count(avfil->outputs); i++){
            if(avfilter_pad_get_type(avfil->outputs, i) != AVMEDIA_TYPE_VIDEO)
                onlyVideo=false;
            }
        }
*/
		bool onlyVideo = true;
        if(onlyVideo){
            if(QString(avfil->name).compare("input") != 0 ){
                scene->filtersSize[QString(avfil->name)] = QSize(
                                            avfilter_pad_count(avfil->inputs),
                                            avfilter_pad_count(avfil->outputs));
            }
        }
        if(QString(avfil->name).compare("split") == 0 ){
            scene->filtersSize[QString(avfil->name)]=QSize(1,2);
        }
    }
}
//------------------------------------------------------------------------------

/**
 * Mouse pressed event handler.
 * @param event
 */

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    unicorn->setGeometry(this->width()+200, this->height()+200, 200,200);
    helpText->setGeometry(this->width()+200, this->height()+200, 200,200);
    QMainWindow::mousePressEvent(event);
}
//------------------------------------------------------------------------------

/**
 * Window resized event handler.
 */

void MainWindow::resizeEvent(QResizeEvent* event)
{
   unicorn->setGeometry(this->width()+200, this->height()+200, 200,200); 
   helpText->setGeometry(this->width()+200, this->height()+200, 200,200);
   QMainWindow::resizeEvent(event);
   // Your code here.
}
//------------------------------------------------------------------------------

/**
 *  Connects two pads
 */

void MainWindow::connectWire(){
    Pad * in = NULL;
    Pad * out = NULL;
	for(auto f : scene->filters){
		if(f->getInPad() != NULL && f->getInPad()->isSelected()){
            in = f->getInPad();
            in->setSelected(false);
            in->setIsSelected(false);
        }else if(f->getInPad2() != NULL && f->getInPad2()->isSelected()){
            in = f->getInPad2();
            in->setSelected(false);
            in->setIsSelected(false);
        }
        if(f->getOutPad() != NULL && f->getOutPad()->isSelected()){
            out = f->getOutPad();
            out->setSelected(false);
            out->setIsSelected(false);
        }else if(f->getOutPad2() != NULL && f->getOutPad2()->isSelected()){
            out = f->getOutPad2();
            out->setSelected(false);
            out->setIsSelected(false);
        }
    }
	
	printf("3\n");
	printf("4\n");
    if( in != NULL && out != NULL && 
		//in->getParent() != out->getParent() && 
		!in->connectedTo() && !out->connectedTo()){
		printf("5\n");
        Wire * w = new Wire(scene);
		printf("6\n");
        w->setConnection(out,in, c);
		printf("7\n");
        c->updateWires(in);
    }
}
//------------------------------------------------------------------------------

void MainWindow::parseTxt(const QString &filename){
    clearAll();
    PipelineParser pp(scene);
    pp.parseTxt(filename);
    view->verticalScrollBar()->setValue(300);
    view->horizontalScrollBar()->setValue(0);
}
//------------------------------------------------------------------------------
    
/**
 *  Starts saving of a pipeline to TXT file
 */

void MainWindow::addInput(){
    QString filename = QFileDialog::getOpenFileName(this,tr("Open xml/txt"),
                                            ".", tr("Files (*.xml *.txt)"));
    if(filename.isEmpty()) return;
    if(filename.endsWith(".txt")) 
        return parseTxt(filename);

    PipelineParser pp(scene);
    pp.parseXml(filename);
}
//------------------------------------------------------------------------------

/**
 * Updates dynamic inspector data.
 */

void MainWindow::updateInspector(){
    
    QApplication::processEvents();
    for(auto w : c->getWires()){
        if(w->getLine()->isSelected()){
            w->updateInspector(insWidget->getTable());
        }
    }
    emit insUpdated();
}
//------------------------------------------------------------------------------

/**
 *  Continues playing after stop.
 */

void MainWindow::justContinue(){
    for(auto p : players){
        removeDock(p);
    }
    players.clear();
}
//------------------------------------------------------------------------------

/**
 * Removes dock widget when previewing finished.
 * @param p
 */

void MainWindow::removeDock(Player * p){
   this->removeDockWidget(p->btn->parentWidget());
}

//------------------------------------------------------------------------------

/**
 * Called when size of frame is received.
 * Initializes preview button size.
 * @param size
 */

void MainWindow::receivedSize(const QSize& size){
	shower->show();
    int minWidth=300>size.width()?size.width():300;
    shower->setGeometry(shower->x(),shower->y(), minWidth,minWidth*
                                        size.height()/size.width());
    
    shower->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    
    shower->setMinimumSize(minWidth, minWidth*size.height()/size.width());
    shower->setMaximumSize(size.width(),size.height());
    shower->setFlat(true);    
}

//------------------------------------------------------------------------------

/**
 * Called when imageis received.
 * Initializes preview button icon.
 * @param size
 */

void MainWindow::receivedImage(const QImage& im){
    shower->setIconSize(QSize(shower->width()-15, shower->height()-15));
    shower->setIcon(QPixmap::fromImage(im).scaledToWidth(shower->width()));
}
//------------------------------------------------------------------------------

/**
 *  Plays preview.
 */

void MainWindow::playVideo(){
    try{
        insWidget->show();
        QDockWidget * dockw = new QDockWidget;
        shower = new QPushButton;
        this->addDockWidget(Qt::RightDockWidgetArea, dockw);
        QThread * nthtr = new QThread;
        
        if(!players.empty()){ 
            justContinue();
            emit doFinish();
            
        }
        
        actualPipeline = new Pipeline(c->inputPath(),  c->computeString());
        PipelineParser pp(actualPipeline, scene, true);
        Player * p = new Player(actualPipeline, shower);
        dockw->setWidget(shower);
        players.push_back(p);
        connect(p, SIGNAL(frameFinishedSig()), this, SLOT(updateInspector()) );
	    connect(this, SIGNAL( insUpdated() ), p ,SLOT(inspectionUpdated()) );
        connect(p, SIGNAL( sendingSize(const QSize&) ), this ,SLOT(receivedSize(const QSize&)) );
        connect(p, SIGNAL( sendingImage(const QImage&) ), this,SLOT(receivedImage(const QImage&)) );
        
        p->moveToThread(nthtr);
        
        connect(this, SIGNAL(doFinish()), p, SLOT(finishIt()) );
        connect(p, SIGNAL(playFinished()), nthtr, SLOT(quit()));
        
        connect(nthtr, SIGNAL(started()), p, SLOT(runPlaying()) );
        connect(nthtr, SIGNAL(finished()), actualPipeline, SLOT(deleteLater2()) );
        connect(nthtr, SIGNAL(finished()), nthtr, SLOT(deleteLater()));
		
        nthtr->start();
       
    }
    catch(std::exception &e){
        QMessageBox information;
        information.setText(e.what());
        information.setWindowTitle("Error");
        information.exec();
    }
}
//------------------------------------------------------------------------------

/**
 * Deprecated
 */

void MainWindow::runInspection(){
    try{
        updateInspector();
    }
    catch(std::exception &e){
        QMessageBox information;
        information.setText(e.what());
        information.setWindowTitle("Error");
        information.exec();
    }
}
//------------------------------------------------------------------------------

/**
  * Returns used ecnoder.
  * @return 
  */

QString MainWindow::getEncoder(){
    for(auto f : scene->filters){
        if(f->getName().compare("output")==0) 
            return f->params[1]->value;
    }
    return QString("none");
}
//------------------------------------------------------------------------------

/**
 * Shows string with text representation
 * of a pipeline.
 */

void MainWindow::showText(){
    try{
        QMessageBox information;
        information.setText( c->computeString());
        information.setWindowTitle("parsed string");
        information.exec();
    }
    catch(std::exception &e){
        QMessageBox information;
        information.setText(e.what());
        information.setWindowTitle("Error");
        information.exec();
    }
}
//------------------------------------------------------------------------------

/**
 * Clears canvas (scene).
 */

void MainWindow::clearAll(){
    for(auto f : scene->filters){
            c->removeWires(f->getInPad(), f->getOutPad());
            if(f->getInPad2())
                c->removeWires(f->getInPad2(), f->getOutPad());
            if(f->getOutPad2())
                c->removeWires(f->getInPad(), f->getOutPad2());
            scene->filters.removeOne(f);
            delete f;
    }	
    scene->update();
}

/**
 * Clears canvas (scene).
 */

void MainWindow::clearAllDefault(){
	this->clearAll();
	QString filename = QString::fromUtf8(default_xml, default_xml_len);
	PipelineParser pp(scene);
	pp.parseXml(filename);
    scene->update();
}
//------------------------------------------------------------------------------
    
/**
 * Shows help for selected item.
 */

void MainWindow::showHelp(){
    
    unicorn->setGeometry(this->width()+200, this->height()+200, 200,200);
    unicorn->raise();
    helpText->hide();
    for(int i = 0; i < 50; i++){
        unicorn->setGeometry(unicorn->pos().x()-8, unicorn->pos().y()-8, 200,200);
        QTimer t;
        t.start((int) (10.0));
        QEventLoop l;
        
        connect( &t, SIGNAL( timeout() ), &l, SLOT( quit() ) );
        l.exec();
    }
    QImage image(":/buble.png");

    QPainter* painter = new QPainter(&image); 
    painter->setPen(Qt::black);
    int fontSize=50;
    
    painter->fontMetrics().boundingRect(helpStr);
    
    QRect r(image.rect().x() + image.width()/4,
            image.rect().y() + image.height()/4,
            image.width()/2,
            image.height()/2);
    
    painter->setFont(QFont("Arial", fontSize));
   
    painter->drawText(r, Qt::TextWordWrap, helpStr);
    painter->end();
    helpText->setPixmap(QPixmap::fromImage(image));
    helpText->setGeometry(unicorn->pos().x()-100, unicorn->pos().y()-100, 250,150);
    helpText->setScaledContents( true ); 
    helpText->show();
    helpText->raise();
}
//------------------------------------------------------------------------------

/**
 * Signalises that arrow cursor was toogled.
 */

void MainWindow::cursTogled(){
    curs->setIcon(QIcon(":/cursorT.png"));
    hand->setIcon(QIcon(":/hand.png"));
    this->view->setDragMode(QGraphicsView::RubberBandDrag);
}
//------------------------------------------------------------------------------

/**
  * Signalises that hand cursor was toogled.
  */

void MainWindow::handTogled(){
    curs->setIcon(QIcon(":/cursor.png"));
    hand->setIcon(QIcon(":/handT.png"));
    this->view->setDragMode(QGraphicsView::ScrollHandDrag);
}
//------------------------------------------------------------------------------

/**
 * Autoconnects filters in pipeline
 */

void MainWindow::autoConnect(){
    try{
        Pad *toConnect;

        QList<Filter*> sorted;
        QList<Filter*> sorted2;
        sorted<<inputPad()->getParent();

        while(!sorted.empty()){
            bool added=false;
            for(auto f : sorted){
                if(!f->getOutPad()){
                    continue;
                }
                if(!f->getOutPad()->connectedTo() &&
                        (toConnect=closestPad(f->getOutPad()))!=NULL){
                    Wire * w = new Wire(scene);
                    w->setConnection(f->getOutPad(),toConnect, c);
                    c->updateWires(f->getOutPad());
                    sorted2.push_back(toConnect->getParent());
                    added=true;
                }
                if(f->getOutPad2()){
                    if(!f->getOutPad2()->connectedTo() &&
                            (toConnect=closestPad(f->getOutPad2()))!=NULL){
                        Wire * w = new Wire(scene);
                        w->setConnection(f->getOutPad2(),toConnect, c);
                        c->updateWires(f->getOutPad2());

                        sorted2.push_back(toConnect->getParent());
                        added=true;
                    }
                } 
            }
            sorted=sorted2;
            if(!added) break;
        }
    }
    catch(std::exception &e){
        QMessageBox information;
        information.setText(e.what());
        information.setWindowTitle("Error");
        information.exec();
    }
}
//------------------------------------------------------------------------------

/**
 * Saves processed video.
 */

void MainWindow::saveVideo(){
   try{
        actualPipeline = new Pipeline(c->inputPath(),  c->computeString());
        PipelineSaver s(actualPipeline, outputName(), getEncoder());   
        s.save();
        delete actualPipeline;
    }
    catch(std::exception &e){
        QMessageBox information;
        information.setText(e.what());
        information.setWindowTitle("Error");
        information.exec();
    }
}
//------------------------------------------------------------------------------

/**
 * Saves pipeline to TXT file.
 */

void MainWindow::saveTXT(){
    QString filename = QFileDialog::getSaveFileName(this,
                                           tr("Save txt"), ".",
                                           tr("Xml files (*.txt)")); 
    if(filename.isEmpty()) return;
    
    
    try{
        PipelineSaver s;
        s.saveTxt(c, filename);
    }
    catch(std::exception &e){
        QMessageBox information;
        information.setText(e.what());
        information.setWindowTitle("Error");
        information.exec();
    }
    
    
}
//------------------------------------------------------------------------------

/**
 * Saves pipeline to XML file.
 */

void MainWindow::saveXML(){
    
    QString filename = QFileDialog::getSaveFileName(this,
                                           tr("Save Xml"), ".",
                                           tr("Xml files (*.xml)"));
    if(filename.isEmpty()) return;
    PipelineSaver s; 
    s.saveXml(scene, c, filename);
    
}
//------------------------------------------------------------------------------

/**
 *  Removes item from canvas (scene).
 */

void MainWindow::removeItem(){
    for(auto w : c->getWires()){
        if(w->getLine()->isSelected())
            c->removeWire(w);
    }
    for(auto f : scene->filters){
        if(f->isSelected()){
            c->removeWires(f->getInPad(), f->getOutPad());
            if(f->getInPad2())
                c->removeWires(f->getInPad2(), f->getOutPad());
            if(f->getOutPad2())
                c->removeWires(f->getInPad(), f->getOutPad2());
            scene->filters.removeOne(f);
            delete f;
        }
    }
    for(auto i : scene->selectedItems()){
        
        scene->removeItem(i);
    }
    scene->update();
}
//------------------------------------------------------------------------------

/**
 * Returns closest pad to pad sent as an argument
 */

Pad * MainWindow::closestPad(Pad * p){
    Pad * pad=NULL;
    double length=1000000.0;
    double lengthNew;
    for(auto f : this->scene->filters){
            if(!f->getInPad()) continue;
            if(f->getOutPad() == p) continue;
            if(f->getOutPad2() == p) continue;
            if(f->getInPad2() != NULL && 
                    f->getInPad2()->getParent() == p->getParent()) continue;
            if(f->getInPad()->getParent() == p->getParent()) continue;
            if((lengthNew=QLineF(p->sceneBoundingRect().center(),
                f->getInPad()->sceneBoundingRect().center()).length())<length && 
                    f->getInPad()->connectedTo()==NULL){

                length=lengthNew;
                pad=f->getInPad();

            }
            if(f->getInPad2()){
                if((lengthNew=QLineF(p->sceneBoundingRect().center(),
                    f->getInPad2()->sceneBoundingRect().center()).length())<length && 
                        f->getInPad2()->connectedTo()==NULL){

                    length=lengthNew;
                    pad=f->getInPad2();

                }
            }
    }   
    return pad;
}
//------------------------------------------------------------------------------

/**
 * Returns output pad of an input filter
 * @return 
 */

Pad * MainWindow::inputPad(){
    Pad * pad=NULL;
    for(auto f : this->scene->filters){
        if(!f->getInPad()) pad=f->getOutPad();
    }   
    if(!pad)
        throw std::runtime_error(" filter.");
    return pad;
}
//------------------------------------------------------------------------------
