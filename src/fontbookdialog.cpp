/***************************************************************************
 *   Copyright (C) 2007 by Pierre Marchand   *
 *   pierre@oep-h.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "fontbookdialog.h"
#include "fontitem.h"
#include "typotek.h"

#include <QFileDialog>
#include <QDebug>
#include <QGraphicsScene>


FontBookDialog::FontBookDialog ( QWidget *parent )
		: QDialog ( parent )
{
	setupUi ( this );
	isOk = false;

	fillSizeList();
	QString alorem ( "Lorem ipsum dolor sit amet, consectetuer adipiscing elit.\nUt a sapien. Aliquam aliquet purus molestie dolor.\nInteger quis eros ut erat posuere dictum. Curabitur dignissim.\nInteger orci. Fusce vulputate lacus at ipsum. \nQuisque in libero nec mi laoreet volutpat." );
	QString loremBig ( "LOREM IPSUM DOLOR" );
	setSampleHeadline ( loremBig );
	setSampleText ( alorem );
	
	m_pageRect = QRectF(0,0,m_pageSize.width(), m_pageSize.height());
	preScene = new QGraphicsScene(m_pageRect);
	preView->setScene(preScene);
	preView->setRenderHint ( QPainter::Antialiasing, true );
	preView->setBackgroundBrush(Qt::lightGray);
	
// 	slotPreview();
	
	connect ( okButton,SIGNAL ( accepted () ),this,SLOT ( slotAccept() ) );
	connect ( okButton,SIGNAL ( rejected() ),this,SLOT ( slotCancel() ) );
	connect ( fileNameButton,SIGNAL ( released() ),this,SLOT ( slotFileDialog() ) );
	connect ( paperSizeCombo,SIGNAL ( activated ( int ) ),this,SLOT ( slotPageSize ( int ) ) );
	connect(this,SIGNAL(updateView()),this,SLOT(slotPreview()));
	
	//all Update
	connect(fileNameEdit,SIGNAL(textChanged( const QString& )),this,SIGNAL(updateView()));
	QList<QSpinBox*> spinList;
	spinList << familySpinBox;
	spinList << styleSpinBox;
	spinList << sampleSpinBox;
	spinList << familyFontSizeSpin;
	spinList << styleFontSizeSpin;
	spinList << headlineFontSizeSpin;
	spinList << bodyFontSizeSpin;
	foreach(QSpinBox *sp, spinList)
	{
		connect(sp,SIGNAL(valueChanged ( int  )),this,SIGNAL(updateView()));
	}
	connect (sampleTextEdit,SIGNAL(textChanged()),SIGNAL(updateView()));
	connect (sampleHeadline,SIGNAL(textChanged( const QString&)),SIGNAL(updateView()));

}


FontBookDialog::~FontBookDialog()
{
}

void FontBookDialog::slotAccept()
{
	isOk = true;
	close();
}

void FontBookDialog::slotCancel()
{
	isOk = false;
	close();
}

void FontBookDialog::slotFileDialog()
{
	QString theFile = QFileDialog::getSaveFileName ( this, "Save fontBook", QDir::homePath() , "Portable Document Format (*.pdf)" );
	fileNameEdit->setText ( theFile );
}

double FontBookDialog::getTabFamily()
{
	return familySpinBox->value();
}

double FontBookDialog::getTabStyle()
{
	return styleSpinBox->value();
}

double FontBookDialog::getTabSampleText()
{
	return sampleSpinBox->value();
}

void FontBookDialog::setSampleText ( QString s )
{
	sampleTextEdit->setPlainText ( s );
}

void FontBookDialog::setSampleHeadline ( QString s )
{
	sampleHeadline->setText ( s );
}

QString FontBookDialog::getSampleHeadline()
{
	return sampleHeadline->text();
}


QString FontBookDialog::getSampleText()
{
	return sampleTextEdit->toPlainText();
}

QString FontBookDialog::getFileName()
{
	return fileNameEdit->text();
}

QSizeF FontBookDialog::getPageSize()
{
	return m_pageSize;

}

void FontBookDialog::fillSizeList()
{
	paperSizeCombo->addItem ( "A0",QSizeF (33.1 * 72.0, 46.8 * 72.0 ) );
	paperSizeCombo->addItem ( "A1",QSizeF (23.4 * 72.0, 33.1 * 72.0 ) );
	paperSizeCombo->addItem ( "A2",QSizeF (16.5 * 72.0, 23.4 * 72.0 ) );
	paperSizeCombo->addItem ( "A3",QSizeF (11.7 * 72.0, 16.5 * 72.0 ) );
	paperSizeCombo->addItem ( "A4",QSizeF ( 8.3 * 72.0, 11.7 * 72.0 ) );
	paperSizeCombo->addItem ( "A5",QSizeF ( 5.8 * 72.0,  8.3 * 72.0 ) );
	// default to A4
	m_pageSize = QSizeF (	8.3  * 72.0, 11.7 * 72.0 );
	m_pageSizeConstant = QPrinter::A4;
	paperSizeCombo->setCurrentIndex ( 4 );

}

double FontBookDialog::getFontSize ( QString s )
{
	if ( s == "family" )
	{
		return familyFontSizeSpin->value();
	}
	if ( s == "style" )
	{
		return styleFontSizeSpin->value();
	}
	if ( s == "headline" )
	{
		return headlineFontSizeSpin->value();
	}
	if ( s == "body" )
	{
		return bodyFontSizeSpin->value();
	}
	return 0.0;
}

void FontBookDialog::slotPageSize ( int index )
{
	QVariant var= paperSizeCombo->itemData ( index );
	if ( var.isValid() )
	{
		m_pageSize = var.toSizeF();
		m_pageRect = QRectF(0,0,m_pageSize.width(), m_pageSize.height());
		
		if(paperSizeCombo->itemText(index)  == "A0")
		{
			m_pageSizeConstant = QPrinter::A0;
		}
		if(paperSizeCombo->itemText(index)  == "A1")
		{
			m_pageSizeConstant = QPrinter::A1;
		}
		if(paperSizeCombo->itemText(index)  == "A2")
		{
			m_pageSizeConstant = QPrinter::A2;
		}
		if(paperSizeCombo->itemText(index)  == "A3")
		{
			m_pageSizeConstant = QPrinter::A3;
		}
		if(paperSizeCombo->itemText(index)  == "A4")
		{
			m_pageSizeConstant = QPrinter::A4;
		}
		if(paperSizeCombo->itemText(index)  == "A5")
		{
			m_pageSizeConstant = QPrinter::A5;
		}
		emit updateView();
		
	}
	else
	{
		qDebug() << "invalid QVARIANT";
	}
}

QPrinter::PageSize FontBookDialog::getPageSizeConstant()
{
	return m_pageSizeConstant;
}

void FontBookDialog::slotPreview()
{
	qDebug() << m_pageRect;
	preScene->setSceneRect(m_pageRect);
	for ( int  n = 0; n < renderedFont.count(); ++n )
	{
		renderedFont[n]->deRenderAll();
				
	}
	renderedFont.clear();
	preScene->removeItem(preScene->createItemGroup(preScene->items()));
	QGraphicsRectItem *backp = preScene->addRect(m_pageRect,QPen(),Qt::white);
	backp->setEnabled ( false );
	
	
	double pageHeight = getPageSize().height();
	double pageWidth =getPageSize().width();
	QString theFile = getFileName();
	double familySize = getFontSize("family");
	double headSize = getFontSize("headline");
	double bodySize = getFontSize("body");
	double styleSize = getFontSize("style");
	double familynameTab = getTabFamily();
	double variantnameTab = getTabStyle();
	double sampletextTab = getTabSampleText();
	double topMargin =  getPageSize().height() / 10.0;
	QStringList loremlist = getSampleText().split ( '\n' );
	QString headline = getSampleHeadline();
	QPrinter::PageSize printedPageSize = getPageSizeConstant();
	double parSize = familySize * 3.0 + styleSize * 1.2 + headSize * 1.2 + static_cast<double>(loremlist.count()) * bodySize * 1.2;
	
	
	QGraphicsScene *theScene = preScene;	
	QString styleString ( QString("color:white;background-color:black;font-family:Helvetica;font-size:%1pt" ).arg(familySize));	
	
	QList<FontItem*> localFontMap = typotek::getInstance()->getCurrentFonts();
	QMap<QString, QList<FontItem*> > keyList;
	for ( int i=0; i < localFontMap.count();++i )
	{
		keyList[localFontMap[i]->value ( "family" ) ].append ( localFontMap[i] );
// 		qDebug() << localFontMap[i]->value ( "family" ) ;
	}
	
	QMap<QString, QList<FontItem*> >::const_iterator kit;
	
	
	
	QPointF pen(0,0);
	QGraphicsTextItem *title;
	QGraphicsTextItem *folio;
	int pageNumber = 0;
	
	
	for ( kit = keyList.begin(); kit != keyList.end(); ++kit )
	{

		pen.rx() = familynameTab;
		pen.ry() += topMargin;

		if ( ( pen.y() + parSize ) > pageHeight * 0.9 )
		{
			preView->fitInView(m_pageRect,Qt::KeepAspectRatio);
			return;
		}
		
		title = theScene->addText ( "" );
		title->setHtml ( QString ( "<span style=\"%2\">%1</span>" ).arg ( kit.key() ).arg ( styleString ) );
		title->setPos ( pen );
		title->setZValue(100.0);
		pen.ry() += 4.0  * familySize;
		
		for ( int  n = 0; n < kit.value().count(); ++n )
		{
// 			qDebug() << "\t\t" << kit.value()[n]->variant();

			if ( ( pen.y() + (parSize - 4.0 * familySize) ) > pageHeight * 0.9 )
			{
				preView->fitInView(m_pageRect,Qt::KeepAspectRatio);
				return;
				
			}
			pen.rx()=variantnameTab;
			FontItem* curfi = kit.value()[n];
			qDebug() << "\tRENDER" << kit.key() << curfi->variant();
			renderedFont.append(curfi);
			curfi->renderLine ( theScene,curfi->variant(), pen ,styleSize );
			pen.rx() = sampletextTab;
			pen.ry() +=  2.0 * styleSize;
			curfi->renderLine ( theScene, headline,pen, headSize );
			pen.ry() +=  headSize * 0.5;
			for ( int l=0; l < loremlist.count(); ++l )
			{
				curfi->renderLine ( theScene, loremlist[l],pen, bodySize );
				pen.ry() +=  bodySize * 1.2;
			}
			pen.ry() +=styleSize * 2.0;

		}
	}
	
	preView->fitInView(m_pageRect,Qt::KeepAspectRatio);

}





