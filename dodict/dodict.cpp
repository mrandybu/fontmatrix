//
// C++ Implementation: dodict
//
// Description: dodict is a little utility that should 
//		help begining a dict file
//
//
// Author: Pierre Marchand <pierremarc@oep-h.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <iostream>

#include <QCoreApplication>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QFile>
#include <QMap>
#include <QRegExp>
#include <QDebug>

QMap<int, QStringList> CatDB;

// Intended to import stuff from m17n project
// This function just takes the _content_ of the "category" list
void importCats(QString path)
{
	QFile cf(path);
	if(!cf.open(QIODevice::ReadOnly | QIODevice::Text ))
	{
		qDebug()<< "Unable to open " << cf.fileName();
		return;
	}
	while (!cf.atEnd()) {
		QByteArray rLine( cf.readLine() );
		QString line(rLine.trimmed());
		if(line.startsWith(';'))
			continue;
		
		for(int i(0);i <line.count(); ++i)
		{
			if(line[i] == ';')
				break;
			int endPar(0);
			if(line[i] == '(')
			{
				while(line[i + endPar] != ')')
					++endPar;
			}
			else
			{
				continue;
			}
			QString record(line.mid(i + 1, endPar - 1));
			QStringList tokenList(record.split(QRegExp("\\s+"), QString::SkipEmptyParts ));
			bool ok;
			if(tokenList.count() == 2)
			{
				int ucs(tokenList[0].toInt(&ok,16));
				CatDB[ucs].append(tokenList[1].mid(1));
				qDebug() <<record<< " : "<< ucs <<" => "<< tokenList[1].mid(1);
			}
			else if(tokenList.count() == 3)
			{
				int ucsB(tokenList[0].toInt(&ok,16));
				int ucsE(tokenList[1].toInt(&ok,16));
				for(int ucs(ucsB);ucs <= ucsE ; ++ucs)
				{
					CatDB[ucs].append(tokenList[2].mid(1));
					qDebug() <<record<< " : "<< ucs <<" => "<< tokenList[2].mid(1);
				}
			}
			else
			{
				qDebug() <<"ERROR RECORD COUNT ("<<tokenList.count()<<") ("<<record<<")";
			}
			i += endPar;
		}	
	}
}

int main(int argc, char *argv[] )
{
	QCoreApplication app( argc, argv );
	QStringList args(app.arguments());
	
	if(args.count() == 1 || args.at(1) == "-h" )
	{
		QString help("dodict \
				\n\t-b \tStart of Unicode interval (decimal)\
				\n\t-e\tEnd of Unicode interval (decimal)\
				\n\t-s\tName of the script (will produce the file script.dict)\
				\n\t-c\tthe file containing the category\n");
		std::cout << help.toStdString() ;
		return 1;
	}
	
	int begin(0);
	int end(0);
	QString lang;
	QString catfile;
// 	bool ok;
	// TODO fix the detection and interpretation of hex strings
	for(int i(1); i < args.count(); ++i)
	{
		if( args.at(i) == "-b" || args.at(i) == "--begin")
		{
			if(args.count() > i + 1)
			{
				QString barg(args.at(i+1));
				begin = barg.toInt();
			}
		}
		if( args.at(i) == "-e" || args.at(i) == "--end")
		{
			if(args.count() > i + 1)
			{
				QString earg(args.at(i+1));
				end = earg.toInt();
			}
		}
		if( args.at(i) == "-s" || args.at(i) == "--script")
		{
			if(args.count() > i + 1)
			{
				lang = args.at(i+1);
			}
		}
		if( args.at(i) == "-c" || args.at(i) == "--catfile")
		{
			if(args.count() > i + 1)
			{
				catfile = args.at(i+1);
			}
		}
	}
	
	if (lang.isEmpty() || begin == 0 || end == 0)
	{
		std::cerr << "\tGame Over!\n\tinsert coin\n";
		return 1;
	}
	else
	{
		QString msgD("Begining of range is :\t" + QString::number(begin));
		QString msgE("End of range is :\t" + QString::number(end));
		QString msgL("Script is :\t" + lang);
		QString msgC("Category file is :\t" + catfile);
		std::cout << msgD.toStdString() << "\n" << msgE.toStdString() << "\n"<< msgL.toStdString() << "\n"<< msgC.toStdString() << "\n";
	}
	
	bool hasDB(false);
	if(!catfile.isEmpty())
	{
		importCats(catfile);
		if(!CatDB.isEmpty())
			hasDB = true;
	}
	
	QFile file(lang + ".dict");
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QString msg("Unable to open "+ file.fileName());
		std::cout << msg.toStdString();
		return 1;
	}

	QTextStream out(&file);
	out << "% Dict file generated by dodict for \"" + lang + "\", please edit it.\n";
	out << "% Records are of the form : FFFF|prop1|prop2\n";
	out << "% Where FFFF is a raw hex value representing a code point in Unicode space\n";
	out << "% and \"props\" are arbitrary ascii strings.\n";
	out << "% Records begining by \"\%\" are comments.\n";
	for(int idx(begin); idx <= end; ++idx)
	{
		QString format("%2");
		QString formatted( format.arg( idx, 4, 16, QChar('0')) );
		
		if(hasDB)
		{
			if(CatDB.contains(idx))
			{
				QStringList cats(CatDB[idx]);
				for(int c(0); c < cats.count() ; ++c)
					formatted += "|" + cats[c];
			}
		}
		
		QString it("\% "+  QString(idx) +" \n"+ formatted +"\n");
		
		out << it ;
	}
	
	file.close();
	std::cout << "Successfully created "<< file.fileName().toStdString() << "\n"; 
	return 0;
}




















