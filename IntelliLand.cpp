//IntelliLand.cpp
#include "IntelliLand.h"

CIntelliLand::CIntelliLand()
{
}

CIntelliLand::~CIntelliLand()
{
}

int CIntelliLand::InitialiseOSMLandDatabase( const QString& dbFilename )
{
	return m_LandDB.InitialiseOSMLandDatabase( dbFilename );
}

void CIntelliLand::FreeDatabase()
{
	m_LandDB.FreeDatabase();
}

NRList CIntelliLand::NameOfOSMLand(double x, double y, int adminLevel)
{
	nearbyResult nrDefault;
	nrDefault.name = "Land";
	NRList landResults;

	while( landResults.size() == 0 && adminLevel > 0 )
		landResults = m_LandDB.NameOfOSMLand(x, y, adminLevel-- );

	//Return something
	if (landResults.size() == 0)
		landResults.push_back(nrDefault);

	return landResults;
}

NRList    CIntelliLand::FullListOfOSMLand(double x, double y)
{
	nearbyResult nrDefault;
	nrDefault.name = "Land";
	NRList landResults;

	landResults = m_LandDB.FullListOfOSMLand(x, y);

	//Return something
	if (landResults.size() == 0)
		landResults.push_back(nrDefault);

	return  landResults;
}

int CIntelliLand::GetBorderingRelations(int relationID, double x, double y, NRList& relResults)
{
	int res = m_LandDB.GetBorderingRelations(relationID, x, y, relResults);
	if (relResults.size() == 0)
	{
		nearbyResult borderResult;
		borderResult.name = "None";
		relResults.push_back(borderResult);
	}

    return res;
}

double CIntelliLand::Distance(CLongLat p1, CLongLat p2)
{
	return m_LandDB.Distance(p1, p2);
}

double CIntelliLand::Bearing(CLongLat p1, CLongLat p2)
{
	return m_LandDB.Bearing(p1, p2);
}


std::string CIntelliLand::GetWikipediaValue(int type, int ID)
{
	return m_LandDB.GetWikipediaValue(type, ID);
}