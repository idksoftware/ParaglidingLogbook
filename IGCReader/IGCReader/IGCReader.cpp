// IGCReader.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>


/**
IGC DATA FILE FORMAT - ORDER OF RECORD TYPES IN AN IGC FILE
*/
enum class RecordType : char {
	A_Record = 'A', // - FR manufacturer and identification(always first)
	H_Record = 'H', // - File header
	I_Record = 'I', // - Fix extension list, of data added at end of each B record
	J_Record = 'J', // - Extension list of data in each K record line
	C_Record = 'C', // - Task / declaration(if used)
	L_Record = 'L', // - Logbook / comments(if used)
	D_Record = 'D', // - Differential GPS(if used)
	F_Record = 'F', // - Initial Satellite Constellation
	B_Record = 'B', // - Fix plus any extension data listed in I Record
	E_Record = 'E', // - Pilot Event(PEV)
	K_Record = 'K', // - Extension data as defined in J Record
	G_Record = 'G', // - Security record(always last)
};

class FlightRecord;

class Utils {
public:
	Utils() = default;
	~Utils() = default;
	static bool FileExists(const char* p);
};

class IGCFile {
public:
	IGCFile() = default;
	~IGCFile() = default;
	bool read(const char* datafile, FlightRecord& FlightRecord);
};

class Split {
public:	
	Split(const char* s);
	~Split() = default;
	
	std::string m_first;
	std::string m_second;
	bool m_found{ false };
};


bool Utils::FileExists(const char* p)
{
#ifdef STDCPP14
	struct stat buffer;
	if (stat(p, &buffer) != 0)
	{
		return false;
	}
	return true;
#else
	std::filesystem::path path = p;
	if (std::filesystem::exists(path) == true) {
		return std::filesystem::is_regular_file(path);
	}
	return false;
#endif
}

Split::Split(const char* s)
{
	std::string text = s;
	int pos = text.find(':');
	if (pos == std::string::npos) {
		return;
	}
	m_first = text.substr(0, pos);
	m_second = text.substr(pos + 1, text.length() - (pos - 2));
}

class A_Record {// - FR manufacturer and identification(always first)
	static std::string m_manufacturer;		// Manufacturer	3 bytes	MMM	Alphanumeric, see para 2.5.6
	static std::string m_uniqueID;			// Unique ID		3 bytes	NNN	Valid characters alphanumeric(AL3)
	static std::string m_idExtension;		// ID extension	Optional	TEXT STRING	Valid characters alphanumeric
public:
	A_Record() = default;
	~A_Record() = default;
	void parse(const char* text);
	static std::string getManufacturer() { return m_manufacturer; };
	static std::string getUniqueID() { return m_uniqueID; };
	static std::string getIDExtension() { return m_idExtension; };
	void print();
};

class H_Record {// - File header
	static std::string m_utcDate; // UTC date this file was recorded
	static std::string m_accuracy; // Fix accuracy in meters, see also FXA three - letter - code reference
	static std::string m_pilot;
	static std::string m_copilot;
	static std::string m_gliderModel;
	static std::string m_gliderRegistration;
	static std::string m_gpsDatum;
	static std::string m_firmwareRevision;
	static std::string m_hardwareRevision;
	static std::string m_manufacturerAndModel;
	static std::string m_gpsManufacturerAndModel;
	static std::string m_pressureSensor;
	static std::string m_gliderCompID;
	static std::string m_gliderCompClass;
	static std::string m_downloadSoftware;
	static std::string m_gnssAltitude;
	static std::string m_pressureMode;
	static std::string m_timeZone;

	
public:
	H_Record() = default;;
	~H_Record() = default;
	void parse(const char* text);
	static std::string getUTCDate() { return m_utcDate; };
	static std::string getAccuracy() { return m_accuracy; };
	static std::string getPilot() { return m_pilot; };
	static std::string getCopilot() { return m_copilot; };
	static std::string getGliderModel() { return m_gliderModel; };
	static std::string getGliderRegistration() { return m_gliderRegistration; };
	static std::string getGPSDatum() { return m_gpsDatum; };
	static std::string getFirmwareRevision() { return m_firmwareRevision; };
	static std::string getHardwareRevision() { return m_hardwareRevision; };
	static std::string getManufacturerAndModel() { return m_manufacturerAndModel; };
	static std::string getGPSManufacturerAndModel() { return m_gpsManufacturerAndModel; };
	static std::string getPressureSensor() { return m_pressureSensor; };
	static std::string getGliderCompID() { return m_gliderCompID; };
	static std::string getGliderCompClass() { return m_gliderCompClass; };
	static std::string getDownloadSoftware() { return m_downloadSoftware; };
	static std::string getGNSSAltitude() { return m_gnssAltitude; };
	static std::string getPressureMode() { return m_pressureMode; };
	static std::string getTimeZone() { return m_timeZone; };
	void print();
};
/*
I record
Description				Size	Element	Remarks
Number of extensions	2 bytes	NN		Valid characters 0-9
Start byte number		2 bytes	SS		Valid characters 0-9
Finish byte number		2 bytes	FF		Valid characters 0-9
3-letter Code			3 bytes	CCC		Alphanumeric, see para 7 for list of codes

*/
class I_Record { // - Fix extension list, of data added at end of each B record
	std::string m_number;			// Number of extensions		2 bytes	NN		Valid characters 0 - 9
	std::string m_startByteNumber;	// Start byte number		2 bytes	SS		Valid characters 0 - 9
	std::string m_finishByteNumber;	// Finish byte number		2 bytes	FF		Valid characters 0 - 9
	std::string m_code;				// 3-letter Code			3 bytes	CCC		Alphanumeric, see para 7 for list of codes
public:
	I_Record(const char* text);
	I_Record() = default;
	std::string getNumber() { return m_number; };
	std::string getStartByteNumber() { return m_startByteNumber; };
	std::string getFinishByteNumber() { return m_finishByteNumber; };
	std::string getCode() { return m_code; };
};

class J_Record { // - Extension list of data in each K record line
public:
	J_Record(const char* text);
	~J_Record() = default;
};

class C_Record { // - Task / declaration(if used)
public:
	C_Record(const char* text);
	~C_Record() = default;
};

class L_Record { // - Logbook / comments(if used)
public:
	L_Record(const char* text);
	~L_Record() = default;
};

class D_Record { // - Differential GPS(if used)
public:
	D_Record(const char* text);
	~D_Record() = default;
};

class F_Record { // - Initial Satellite Constellation
public:
	F_Record(const char* text);
	~F_Record() = default;
};

class B_Record { // - Fix plus any extension data listed in I Record
	std::string m_timeUTC;
	std::string m_latitude;
	std::string m_longitude;
	std::string m_fixValidity;
	std::string m_pressAlt;
	std::string m_gnssAlt;
public:
	B_Record(const char* text);
	B_Record() = default;
	std::string getTimeUTC() { return m_timeUTC; };
	std::string getLatitude() { return m_latitude; };
	std::string getLongitude() { return m_longitude; };
	std::string getFixValidity() { return m_fixValidity; };
	std::string getPressAlt() { return m_pressAlt; };
	std::string getGNSSAlt() { return m_gnssAlt; };

	void print();
};

class E_Record { // - Pilot Event(PEV)
public:
	E_Record(const char* text);
	~E_Record() = default;
};

class K_Record { // - Extension data as defined in J Record
public:
	K_Record(const char* text);
	~K_Record() = default;
};

class G_Record { // - Security record(always last)
public:
	G_Record(const char* text);
	~G_Record() = default;
};

/*
Manufacturer	3 bytes	MMM	Alphanumeric, see para 2.5.6
Unique ID		3 bytes	NNN	Valid characters alphanumeric(AL3)
ID extension	Optional	TEXT STRING	Valid characters alphanumeric
*/

std::string A_Record::m_manufacturer;		// Manufacturer	3 bytes	MMM	Alphanumeric, see para 2.5.6
std::string A_Record::m_uniqueID;			// Unique ID		3 bytes	NNN	Valid characters alphanumeric(AL3)
std::string A_Record::m_idExtension;		// ID extension	Optional	TEXT STRING	Valid characters alphanumeric

void A_Record::parse(const char* text) {
	std::string rec = text;
	std::string type = rec.substr(1, 3);
	if (type.compare("MMM") == 0) {// Manufacturer
		m_manufacturer = rec.substr(4, rec.length() - 4);
		return;
	}
	if (type.compare("NNN") == 0) {// Unique ID
		m_uniqueID = rec.substr(4, rec.length() - 4);
		return;
	}
	m_idExtension = rec.substr(1, rec.length() - 2);
}

void A_Record::print() {
	if (getManufacturer().length() != 0) { printf("Manufacturer: %s\n", getManufacturer().c_str()); }
	if (getUniqueID().length() != 0) { printf("UniqueID: %s\n", getUniqueID().c_str()); }
	if (getIDExtension().length() != 0) { printf("IDExtension: %s\n", getIDExtension().c_str()); }
}

/*
* HFPLTPILOT
* 
*	Record	Format								Sample									Summary
	HFDTE	HFDTEDDMMYY							HFDTE280709								UTC date this file was recorded
												for 28th July 2009	

	HFFXA	HFFXAAAA							HFFXA035
												fix accuracy 35 meters					Fix accuracy in meters, see also FXA three-letter-code reference

	HFPLT	HFPLTPILOTINCHARGE:TEXTSTRING		HFPLTPILOTINCHARGE: Bloggs, Bill D		Name of the competing pilot

	HFCM2	HFCM2CREW2:TEXTSTRING				HFCM2CREW2: Smith-Barry John A			Name of the second pilot in a two-seater

	HFGTY	HFGTYGLIDERTYPE:TEXTSTRING			HFGTYGLIDERTYPE: Schleicher ASH-25		Free-text name of the glider model

	HFGID	HFGIDGLIDERID:TEXTSTRING			HFGIDGLIDERID: N116 EL					Glider registration number, e.g. N-number

	HFDTM	HFDTMNNNGPSDATUM:TEXTSTRING			HFDTM100GPSDATUM: WGS-1984				GPS datum used for the log points - use igc code 100 / WGS84 unless you are insane.

	HFRFW	HFRFWFIRMWAREVERSION:TEXTSTRING		HFRFWFIRMWAREVERSION:6.4				Any free-text string descibing the firmware revision of the logger

	HFRHW	HFRHWHARDWAREVERSION:TEXTSTRING		HFRHWHARDWAREVERSION:3.0				Any free-text string giving the hardware revision number of the logger

	HFFTY	HFFTYFRTYPE:MANUFACTURERSNAME,FRMODELNUMBER	
												HFFTYFRTYPE: Cambridge, FunkyLogger 77	Logger free-text manufacturer and model

	HFGPS	HFGPSMANUFACTURER,MODEL,CHANNELS,MAXALT	
												HFGPSMarconi,SuperX,12ch,10000m
																						Manufacturer and model of the GPS receiver used in the logger. I think there's a TYPO in this
																									section of the FAI document (2008), suggesting a colon is needed.

	HFPRS	HFPRSPRESSALTSENSOR:MANUFACTURER,MODEL,MAXALT	
												HFPRSPRESSALTSENSOR: Sensyn,A32,11000m	Free-text (separated by commas) description of the pressure sensor used in the logger

	HFCID	HFCIDCOMPETITIONID: TEXTSTRING		HFCIDCOMPETITIONID: B21					The fin-number by which the glider is generally recognised

	HFCCL	HFCCLCOMPETITIONCLASS: TEXTSTRING	HFCCLCOMPETITIONCLASS:15m Motor Glider	Any free-text description of the class this glider is in, e.g. Standard, 15m, 18m, Open.

	Non-IGC FRs. For Non-IGC FRs, the following H-records apply :

	HOSOF		As required. To describe software name, version and date/time of download


	HFALG		ELL for WGS84 Ellipsoid (mandatory zerodatum for IGC FRs)
				GEO for WGS84 Geoid (approx Sea Level datum)
				NKN = GNSS altitude datum not known
				NIL = GNSS altitude not recorded, In which case B records must have V for the fix
							validity and 00000 for GNSS altitude

	HFALP		ISA = ICAO ISA
				MSL = Above Mean Sea Level
				NKN = Pressure altitude datum not known
				NIL = Pressure Alt not recorded, and 00000 must appear in IGC file
	*/


std::string H_Record::m_utcDate; // UTC date this file was recorded
std::string H_Record::m_accuracy; // Fix accuracy in meters, see also FXA three - letter - code reference
std::string H_Record::m_pilot;
std::string H_Record::m_copilot;
std::string H_Record::m_gliderModel;
std::string H_Record::m_gliderRegistration;
std::string H_Record::m_gpsDatum;
std::string H_Record::m_firmwareRevision;
std::string H_Record::m_hardwareRevision;
std::string H_Record::m_manufacturerAndModel;
std::string H_Record::m_gpsManufacturerAndModel;
std::string H_Record::m_pressureSensor;
std::string H_Record::m_gliderCompID;
std::string H_Record::m_gliderCompClass;
std::string H_Record::m_downloadSoftware;
std::string H_Record::m_gnssAltitude;
std::string H_Record::m_pressureMode;
std::string H_Record::m_timeZone;

void H_Record::parse(const char* text) {
	std::string rec = text;
	std::string type = rec.substr(0, 5);
	if (type.compare("HFDTE") == 0) {// UTC date this file was recorded
		m_utcDate = rec.substr(5, rec.length() - 4);
		return;
	}
	if (type.compare("HFFXA") == 0) {// Fix accuracy in meters, see also FXA three-letter-code reference
		m_accuracy = rec.substr(5, rec.length() - 4);
		return;
	}
	Split split(text);
	if (type.compare("HOSOF") == 0) {// Fix accuracy in meters, see also FXA three-letter-code reference
		m_downloadSoftware = split.m_second;
		return;
	}
	if (type.compare("HFALG") == 0) {// Fix accuracy in meters, see also FXA three-letter-code reference
		m_gnssAltitude = split.m_second;
		return;
	}
	if (type.compare("HFALP") == 0) {// Fix accuracy in meters, see also FXA three-letter-code reference
		m_pressureMode = split.m_second;
		return;
	}
	
	if (type.compare("HFPLT") == 0) {	// Name of the competing pilot
		m_pilot = split.m_second;
		return;
	}
	if (type.compare("HFCM2") == 0) {	// Name of the second pilot in a two-seater
		m_copilot = split.m_second;
		return;
	}
	if (type.compare("HFGTY") == 0) {// Free-text name of the glider model
		m_gliderModel = split.m_second;
		return;
	}
	if (type.compare("HFGID") == 0) {// Glider registration number, e.g. N-number
		m_gliderRegistration = split.m_second;
		return;
	}
	if (type.compare("HFDTM") == 0) {// GPS datum used for the log points - use igc code 100 / WGS84 unless you are insane.
		m_gpsDatum = split.m_second;
		return;
	}
	if (type.compare("HFRFW") == 0) {// Any free-text string descibing the firmware revision of the logger
		m_firmwareRevision = split.m_second;
		return;
	}
	if (type.compare("HFRHW") == 0) {// Any free-text string giving the hardware revision number of the logger
		m_hardwareRevision = split.m_second;
		return;
	}
	if (type.compare("HFFTY") == 0) {// Logger free-text manufacturer and model
		m_manufacturerAndModel = split.m_second;
		return;
	}
	if (type.compare("HFGPS") == 0) {// Manufacturer and model of the GPS receiver used in the logger. I think there's a TYPO in this
																// section of the FAI document(2008), suggesting a colon is needed.
		m_gpsManufacturerAndModel = split.m_second;
		return;
	}
	
	if (type.compare("HFPRS") == 0) {// Free-text (separated by commas) description of the pressure sensor used in the logger
		m_pressureSensor = split.m_second;
		return;
	}
	if (type.compare("HFCID") == 0) {// The fin-number by which the glider is generally recognised
		m_gliderCompID = split.m_second;
		return;
	}
	if (type.compare("HFCCL") == 0) {// Any free-text description of the class this glider is in, e.g. Standard, 15m, 18m, Open.
		m_gliderCompClass = split.m_second;
		return;
	}
	if (type.compare("HFTZN") == 0) {// Any free-text description of the class this glider is in, e.g. Standard, 15m, 18m, Open.
		m_timeZone = split.m_second;
		return;
	}

}

void H_Record::print()
{
	if (getUTCDate().length() != 0) { printf("UTC Date: %s\n", getUTCDate().c_str()); }
	if (getAccuracy().length() != 0) { printf("Accuracy: %s\n", getAccuracy().c_str()); }
	if (getPilot().length() != 0) { printf("Pilot: %s\n", getPilot().c_str()); }
	if (getCopilot().length() != 0) { printf("Copilot: %s\n", getCopilot().c_str()); }
	if (getGliderModel().length() != 0) { printf("Glider Model: %s\n", getGliderModel().c_str()); }
	if (getGliderRegistration().length() != 0) { printf("Glider Registration: %s\n", getGliderRegistration().c_str()); }
	if (getGPSDatum().length() != 0) { printf("GPS Datum: %s\n", getGPSDatum().c_str()); }
	if (getFirmwareRevision().length() != 0) { printf("Firmware Revision: %s\n", getFirmwareRevision().c_str()); }
	if (getHardwareRevision().length() != 0) { printf("Hardware Revision: %s\n", getHardwareRevision().c_str()); }
	if (getManufacturerAndModel().length() != 0) { printf("Manufacturer and Model: %s\n", getManufacturerAndModel().c_str()); }
	if (getGPSManufacturerAndModel().length() != 0) { printf("GPS Manufacturer and Model: %s\n", getGPSManufacturerAndModel().c_str()); }
	if (getPressureSensor().length() != 0) { printf("Pressure Sensor: %s\n", getPressureSensor().c_str()); }
	if (getGliderCompID().length() != 0) { printf("Glider Comp ID: %s\n", getGliderCompID().c_str()); }
	if (getGliderCompClass().length() != 0) { printf("Glider Comp Class: %s\n", getGliderCompClass().c_str()); }
	if (getDownloadSoftware().length() != 0) { printf("Download Software: %s\n", getDownloadSoftware().c_str()); }
	if (getGNSSAltitude().length() != 0) { printf("GNSS Altitude: %s\n", getGNSSAltitude().c_str()); }
	if (getPressureMode().length() != 0) { printf("Pressure Mode: %s\n", getPressureMode().c_str()); }
	if (getTimeZone().length() != 0) { printf("Time Zone: %s\n", getTimeZone().c_str()); }
}

I_Record::I_Record(const char* text) {

}

J_Record::J_Record(const char* text) {

}

C_Record::C_Record(const char* text) {

}

L_Record::L_Record(const char* text) {

}

D_Record::D_Record(const char* text) {

}

F_Record::F_Record(const char* text) {

}
/*
*
B record - Description	Size	Element		Remarks

Time UTC				6 bytes	HHMMSS		Valid characters 0-9. When a valid GNSS fix is received, the UTC time in a B-record line must be obtained
											directly from the same GNSS data package that was the source of the Lat/long and GNSS altitude that is recorded in the same B-record line. Other sources for the time in a B-record line (such as the Real-Time Clock in the recorder) must only be used to provide time-continuity where GNSS fixes are not available(AL9)

Latitude				8 bytes	DDMMmmm	N/S	Valid characters N, S, 0-9. Obtained directly from the same GPS data package that was the source of the
											UTC time that is recorded in the same B-record line. If no latitude is obtained from satellite data, pressure altitude fixing must continue, using times from the RTC. In this case, in B record lines must repeat the last latitude that was obtained from satellite data, until GPS fixing is regained.(AL11)

Longitude				9 bytes	DDDMMmmmE/W	Valid characters E,W, 0-9. Obtained directly from the same GPS data package that was the source of UTC time
											that is recorded in the same B-record line. If no longitude is obtained from satellite data, pressure altitude
											fixing must continue, using times from the RTC. In this case, in B record lines must repeat the last longitude
											that was obtained from satellite data, until GPS fixing is regained.(AL11)

Fix validity			1 byte.	A or V		Use A for a 3D fix and V for a 2D fix (no GPS altitude) or for no GPS data (pressure altitude data must continue
											to be recorded using times from the RTC) .(AL11)

Press Alt.				5 bytes	PPPPP		Altitude to the ICAO ISA above the 1013.25 HPa sea level datum, valid characters 0-9 and negative sign "-".
											Negative values to have negative sign instead of leading zero

GNSS Alt.				5 bytes	GGGGG		Altitude above the WGS84 ellipsoid, valid characters 0-9
*/



B_Record::B_Record(const char* s) {
	// B 124650 5052990N 00013032W A 00199 00189
	std::string text = s;
	text = text.substr(1, text.length() - 1);

	m_timeUTC = text.substr(0, 6);
	text = text.substr(6, text.length() - 1);

	m_latitude = text.substr(0, 8);
	text = text.substr(8, text.length() - 1);

	m_longitude = text.substr(0, 9);
	text = text.substr(9, text.length() - 1);

	m_fixValidity = text.substr(0, 1);
	text = text.substr(1, text.length() - 1);

	m_pressAlt = text.substr(0, 5);
	text = text.substr(5, text.length() - 1);

	m_gnssAlt = text.substr(0, 5);
	text = text.substr(5, text.length() - 1);
}

static std::vector <std::shared_ptr<B_Record>> s_BRecords{ nullptr };

void B_Record::print()
{
	printf("Time: %s ", B_Record::getTimeUTC().c_str());
	printf(" Lat: %s ", B_Record::getLatitude().c_str());
	printf(" Long: %s ", B_Record::getLongitude().c_str());
	printf(" Fix: %s ", B_Record::getFixValidity().c_str());
	printf(" Press: %s", B_Record::getPressAlt().c_str());
	printf(" GNSS: %s\n", B_Record::getGNSSAlt().c_str());
}

E_Record::E_Record(const char* text) {

}

K_Record::K_Record(const char* text) {

}

G_Record::G_Record(const char* text) {

}

class FlightRecord {
	/*
	SINGLE INSTANCE DATA RECORDS
	A record - FR ID number
	G record - Security
	H record - File header
	I record - Extension to the fix(B) record
	J record - Extension to the K record
	C record - Task
	*/
	std::shared_ptr<A_Record> m_aRecord{ nullptr };
	std::shared_ptr<H_Record> m_hRecord{ nullptr };
	/*
	MULTIPLE INSTANCE DATA RECORDS
	B record - Fix
	E record - Events
	F record - Satellite constellation
	K record - Data needed less frequently than fixes
	L record - Log book / comments
	D record - Differential GPS
	*/
	std::vector<std::shared_ptr<B_Record>> m_bRecords;
public:
	FlightRecord();
	~FlightRecord() = default;
	void print();
	void setARecord(A_Record& rec) {
		m_aRecord = std::make_shared<A_Record>(rec);
	}
	void setHRecord(H_Record& rec) {
		m_hRecord = std::make_shared<H_Record>(rec);
	}
	void insertBRecord(B_Record& rec) {
		m_bRecords.push_back(std::make_shared<B_Record>(rec));
	}
};

FlightRecord::FlightRecord() {
	
	
}

void FlightRecord::print() {
	printf("A_Record\n");
	m_aRecord->print();
	printf("H_Record\n");
	m_hRecord->print();
	printf("B_Record\n");
	for (auto item : m_bRecords) {
		item->print();
	}

}

bool IGCFile::read(const char* datafile, FlightRecord& flightRecord) {
	std::string text;

	std::ifstream file(datafile);
	if (file.is_open() == false) {
return false;
	}
	bool res = true;
	H_Record hRecord;
	A_Record aRecord;
	while (std::getline(file, text)) {
		char recordTypeChar = text[0];
		//printf("Record type: %c\n", recordTypeChar);
		switch ((RecordType)recordTypeChar) {
		case RecordType::A_Record: // - FR manufacturer and identification(always first)
			aRecord.parse(text.c_str());
			break;
		case RecordType::H_Record: // - File header
			hRecord.parse(text.c_str());
			break;
		case RecordType::I_Record: // - Fix extension list, of data added at end of each B record
			break;
		case RecordType::J_Record: // - Extension list of data in each K record line
			break;
		case RecordType::C_Record: // - Task / declaration(if used)
			break;
		case RecordType::L_Record: // - Logbook / comments(if used)
			break;
		case RecordType::D_Record: // - Differential GPS(if used)
			break;
		case RecordType::F_Record: // - Initial Satellite Constellation
			break;
		case RecordType::B_Record: // - Fix plus any extension data listed in I Record
		{
			B_Record bRecord(text.c_str());
			flightRecord.insertBRecord(bRecord);
			break;
		}
		case RecordType::E_Record: // - Pilot Event(PEV)
			break;
		case RecordType::K_Record: // - Extension data as defined in J Record
			break;
		case RecordType::G_Record: // - Security record(always last)
			break;
		}
	}
	flightRecord.setARecord(aRecord);
	flightRecord.setHRecord(hRecord);
	file.close();
	return res;
}



class LodbookSummary
{
	std::string m_numberOfFlights;
	std::string m_flyingHours;
	std::string m_totalKm;
	std::string m_averageFlightTime;
	std::string m_averageTrackLenght;
	std::vector <std::string> m_wings;
	std::vector <std::string> m_flyingLocation;
public:
	LodbookSummary() = default;
	~LodbookSummary() = default;
};

class YearStatistics {
	int m_flightDays;
	int m_flightHours;
	int m_numberOfFlights;
public:
	YearStatistics() = default;
	~YearStatistics() = default;
};

class AnnualStatistics : std::vector <std::shared_ptr<YearStatistics>> {
public:
	AnnualStatistics() = default;
	~AnnualStatistics() = default;
};

class FlightSummary {
	std::string m_location;
	std::string m_date;
	std::string m_wing;
	std::string m_duration;
	std::string m_maxDistance;
	std::string m_maxAltitude;
	std::string m_trackLength;
	std::string m_averageSpeed;
	std::string m_altitudeGain;
public:
	FlightSummary() = default;
	~FlightSummary() = default;
};





int main(int argc, char* argv[])
{
	if (argc < 2) {
		return -1;
	}
	if (argv[1] == nullptr) {
		return -1;
	}
	if (Utils::FileExists(argv[1]) == false) {
		return -1;
	}
	std::string path = argv[1];
	printf("File: %s", argv[1]);
	IGCFile IGCFile;
	FlightRecord flightRecord;
	if (IGCFile.read(path.c_str(), flightRecord) == false)
	{
		return -1;
	}
	flightRecord.print();
	
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
