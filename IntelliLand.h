//IntelliLand.h
#pragma once
#include "LandDB.h"


class CIntelliLand
{
//Constructor
public:
	CIntelliLand();
	~CIntelliLand();

	//Methods
public:
	int InitialiseOSMLandDatabase( const QString& dbbFilename );
	void FreeDatabase();
	NRList   NameOfOSMLand(double x, double y, int adminLevel);
	NRList   FullListOfOSMLand(double x, double y);
	int GetBorderingRelations(int relationID, double x, double y, NRList& relResults);
	double Distance(CLongLat p1, CLongLat p2);
	double Bearing(CLongLat p1, CLongLat p2);
	std::string  GetWikipediaValue(int type, int ID);


//Attributes
private:
	CLandDB m_LandDB;
};

