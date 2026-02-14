//WKTConverter.cpp
#include "WKTConverter.h"

QList<QGeoCoordinate> CWKTConverter::ParseCoordinates(const QString& coordinates )
{
    QList<QGeoCoordinate> geoCoordinates;
    QString trimmedCoordinates = coordinates.trimmed();

    if (trimmedCoordinates.isEmpty()) 
    {
        qDebug() << "WktParserUtils::parseCoordinateString: Input string is empty.";
        return QList<QGeoCoordinate>(); // Return empty list for empty input
    }

    // Split the string into individual coordinate pairs
    QStringList coordinatePairs = trimmedCoordinates.split(',', Qt::SkipEmptyParts);

    for (const QString& coordinatePair : coordinatePairs ) 
    {
        QString currentPair = coordinatePair.trimmed();

        if (currentPair.isEmpty()) 
        {
            qWarning() << "WKTConverter:::ParseCoordinates : Found an empty coordinate pair. Returning empty list.";
            return QList<QGeoCoordinate>();
        }

        // Split each pair into constituent numbers 
        QStringList values = currentPair.split(' ', Qt::SkipEmptyParts);

        if (values.size() != 2) 
        {
            qWarning() << "WKTConverter::ParseCoordinates: Expected exactly two values (longitude latitude) but found"
                << values.size() << "for pair:" << currentPair<< ". Returning empty list.";
            return QList<QGeoCoordinate>(); 
        }

        bool okLongitude = false;
        double longitude = values.at(0).toDouble(&okLongitude);

        bool okLatitude = false;
        double latitude = values.at(1).toDouble(&okLatitude);

        if (!okLongitude || !okLatitude) 
        {
            qWarning() << "WKTConverter::ParseCoordinates: Failed to convert numeric values for pair:"
                << currentPair<< ". Returning empty list.";
            return QList<QGeoCoordinate>(); 
        }

        // QGeoCoordinate expects (latitude, longitude)
        geoCoordinates.append(QGeoCoordinate(latitude, longitude));
    }

    return geoCoordinates;
}

QGeoPolygon CWKTConverter::ParsePolygon(const QString& polygonString)
{
    QGeoPolygon geoPolygon;
    QString polygonBlocks = polygonString.trimmed();

    if (polygonBlocks.isEmpty())
    {
        qWarning() << "WKTConverter::ParsePolygon: Input polygon string is empty.";
        return QGeoPolygon(); // Return an invalid/empty QGeoPolygon
    }

    // Basic check for the expected WKT Polygon coordinate format: "((...))"
    // This function assumes the "POLYGON " prefix is already stripped 
    if (!polygonBlocks.startsWith("((") || !polygonBlocks.endsWith("))"))
    {
        qWarning() << "WKTConverter::ParsePolygon: Invalid WKT Polygon string format. Expected to start with '((' and end with '))'.";
        return QGeoPolygon();
    }

    // Remove the outermost '(( ))' to get the content of all rings
    QString innerContent = polygonBlocks.mid(1, polygonBlocks.length() - 2);

    // Split the content into individual ring strings.
    QStringList rings = innerContent.split("), (", Qt::SkipEmptyParts);

    if (rings.isEmpty())
    {
        qWarning() << "WKTConverter::ParsePolygon: No rings found in the polygon string content.";
        return QGeoPolygon();
    }


    // The first string in the list represents the outer ring
    QString outerRing = rings.first();
    //If there are multiple rings the split operation removes the trailing close bracket and leading open bracket 
    if( rings.size() > 1 )
        outerRing.append( ')');

    if (!outerRing.startsWith("(") || !outerRing.endsWith(")"))
    {
        qWarning() << "WKTConverter::ParsePolygon: First ring does not start and finish with brackets.";
        return QGeoPolygon();
    }

    //Strip the outer brackets to get just the coordinates 
    QString coordinates = outerRing.mid(1, outerRing.length() - 2);
    QList<QGeoCoordinate> outerRingPath = ParseCoordinates(coordinates );

    if (outerRingPath.isEmpty())
    {
        qWarning() << "WKTConverter::ParsePolygon: Failed to parse outer ring coordinates. Returning invalid polygon.";
        return QGeoPolygon();
    }

    geoPolygon.setPerimeter(outerRingPath );

    // Process any subsequent strings as inner rings (holes)
    for (int i = 1; i < rings.size(); ++i)
    {
        QString innerRing = rings.at(i);

        //The split function does not remove the closing bracket for the last inner ring
        if( i = rings.size() - 1 )
        innerRing.chop(1);
        QList<QGeoCoordinate> innerRingPath = ParseCoordinates(innerRing );

        if (!innerRingPath.isEmpty())
        {
            geoPolygon.addHole(innerRingPath);
        }
        else
        {
            qWarning() << "WKTConverter::ParsePolygon: Failed to parse inner ring coordinates for ring:" << innerRing<< ". This inner ring will be skipped.";
        }
    }

    return geoPolygon;
}

QList<QGeoPolygon> CWKTConverter::ParseMultipolygon(const QString& multiPolygonString)
{
    QList<QGeoPolygon> geoPolygons;
    QString trimmedInput = multiPolygonString.trimmed();

    if (trimmedInput.isEmpty()) 
    {
        qWarning() << "WKTConverter::ParseMultipolygon: Input multipolygon string is empty.";
        return QList<QGeoPolygon>();
    }

    // A MultiPolygon string (without the "MULTIPOLYGON " prefix) typically
    // starts and ends with a single '(' and ')' that enclose all polygon definitions.
    // e.g., "(((poly1 outer), (poly1 inner))), (((poly2 outer)))"
    if (!trimmedInput.startsWith( "(((") || !trimmedInput.endsWith( ")))" )) 
    {
        qWarning() << "WKTConverter::ParseMultipolygon: Invalid WKT MultiPolygon format. Expected outermost '(((' and ')))'.";
        return QList<QGeoPolygon>();
    }

    // Remove the outermost '(' and ')' to get the content containing individual polygons
    QString polygonsContent = trimmedInput.mid(1, trimmedInput.length() - 2).trimmed();

    if (polygonsContent.isEmpty()) 
    {
        qDebug() << "WKTConverter::ParseMultipolygon: No polygon content found inside outermost parentheses.";
        return QList<QGeoPolygon>();
    }

    int depth = 0; // Tracks the depth of parentheses for nested structures
    int currentPolygonStart = 0; // Start index of the current individual polygon string

    // Iterate through the content to extract each top-level polygon block
    for (int i = 0; i < polygonsContent.length(); ++i) 
    {
        QChar c = polygonsContent.at(i);

        if (c == '(') 
        {
            depth++;
        }
        else if (c == ')') {
            depth--;
        }

        // A complete polygon string is identified when:
        // 1. The parenthesis balance returns to 0 meaning a full '((...))' block is potentially closed.
        // 2. We are at the end of the 'polygonsContent' string OR the next character is a comma (',')
        //    which separates individual polygons at the top level of the MultiPolygon.
        if (depth == 0 && (i + 1 == polygonsContent.length() || polygonsContent.at(i + 1) == ',')) 
        {
            QString polygonBlock = polygonsContent.mid(currentPolygonStart, i - currentPolygonStart + 1).trimmed();

            // Use the existing ParsePolygon method for the extracted block
            QGeoPolygon geoPolygon = ParsePolygon(polygonBlock);
            if (geoPolygon.isValid()) 
            {
                geoPolygons.append(geoPolygon);
            }
            else 
            {
                qWarning() << "WKTConverter::ParseMultipolygon: Generated an invalid polygon" << polygonBlock << ". Skipping this polygon.";
            }

            // Move 'currentPolygonStart' to the beginning of the next polygon block.
            // Skip the current polygon block and the comma (if present).
            currentPolygonStart = i + 2; // +1 for the current char, +1 for the potential comma
            // Adjust the loop index 'i' to correctly start the next iteration
            i = currentPolygonStart - 1;
        }
    }

    // Final check for unbalanced parentheses in the entire multipolygon content
    if (depth!= 0) 
    {
        qWarning() << "WKTConverter::ParseMultipolygon: Unbalanced parentheses in multipolygon content. Result might be incomplete.";
        return QList<QGeoPolygon>(); 
    }

    return geoPolygons;
}

bool CWKTConverter::ParseWKTToGeoPolygons(CGeoResult& geoResult)
{
    QString wkt = QString( geoResult.m_wkt.c_str() );
    QString trimmedWkt = wkt.trimmed();

    if (trimmedWkt.isEmpty()) 
    {
        qWarning() << "WKTConverter::ParseWktToGeoPolygons: Input WKT string is empty.";
        return false;
    }

    QString dataString;
    bool parsingSuccess = false; // Tracks if any polygons were successfully parsed and added

    // Check for POLYGON type
    if (trimmedWkt.startsWith("polygon", Qt::CaseInsensitive))
    {
        dataString = trimmedWkt.mid(QString("polygon").length()).trimmed();
        QGeoPolygon parsedPolygon = ParsePolygon(dataString);
        if (parsedPolygon.isValid()) 
        {
            geoResult.m_geoPolygons.append(parsedPolygon);
            parsingSuccess = true;
        }
        else {
            qWarning() << "WKTConverter::ParseWktToGeoPolygons: Failed to parse POLYGON data.";
        }
    }
    // Check for MULTIPOLYGON type
    else if (trimmedWkt.startsWith("multipolygon",Qt::CaseInsensitive))
    {
        dataString = trimmedWkt.mid(QString("multipolygon").length()).trimmed();
        QList<QGeoPolygon> parsedMultipolygons = ParseMultipolygon(dataString);
        if (!parsedMultipolygons.isEmpty()) 
        {
            geoResult.m_geoPolygons.append(parsedMultipolygons); // Appends all elements from the list
            parsingSuccess = true;
        }
        else 
        {
            qWarning() << "WKTConverter::ParseWktToGeoPolygons: Failed to parse MULTIPOLYGON data or no valid polygons found.";
        }
    }
    // Handle unsupported or malformed WKT types
    else 
    {
        qWarning() << "WKTConverter::ParseWktToGeoPolygons: Unsupported WKT geometry type or malformed string start:" << trimmedWkt.left(30) << "...";
        parsingSuccess = false; // No recognized type, so no polygons parsed
    }

    return parsingSuccess;
}

bool CWKTConverter::ParseGeoResults(QList<CGeoResult>& geoResults)
{
    bool parseSuccess = false;

    //Iterate through the geoResults and parse each one
    for (CGeoResult& geoResult : geoResults )
    {
        bool success = ParseWKTToGeoPolygons(geoResult);
        if (success)
            parseSuccess = true;
    }

    return parseSuccess;
}
