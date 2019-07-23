// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include<string>
#include<vector>
#include<fstream>
using namespace std;

const int SUCCESS = 1;
const int INVALID_USER = 2;
const int INVALID_PASSWORD = 3;
const int INVALID_TICKET = 4;
const int INVALID_SERVICETYPE = 5;
const int INVALID_SERVICEID = 6;
const int INVALID_CLOCKINTIME = 7;
const int INVALID_CLOCKOUTTIME = 8;
const int NO_PERMISSION = 9;
const int TICKET_NOT_ASSIGNED = 10;
const int SYSTEM_ERROR = 11;
const string FILE_PATH = "C:\\Happy\\CPP\\ServiceTracker\\ConsoleApplication1\\Debug\\";


class BaseObject
{
protected:
	string id;
public:
	BaseObject()
	{
		id = "";
	}
	virtual ~BaseObject()
	{

	}

	string getID()
	{
		return this->id;
	}
	void setID(string _id)
	{
		this->id = _id;
	}
};

class ListOfBaseObject : public vector<BaseObject *>
{
public:
	ListOfBaseObject()
	{

	}
	virtual ~ListOfBaseObject()
	{

	}

	void add(BaseObject *baseobject)
	{
		this->push_back(baseobject);
	}
	BaseObject* find(string _id)
	{
		int size = this->size();
		for (int iloop = 0; iloop < size; iloop++)
		{
			BaseObject* baseObject = this->at(iloop);

			if (baseObject->getID().compare(_id) == 0)
				return baseObject;
		}

		return NULL;
	}
	bool isexits(string _id)
	{
		int size = this->size();
		for (int iloop = 0; iloop < size; iloop++)
		{
			BaseObject* baseObject = this->at(iloop);

			if (baseObject->getID().compare(_id) == 0)
				return true;
		}

		return false;
	}
	BaseObject* get(string _id)
	{
		BaseObject *item = this->find(_id);

		if (item == NULL)
		{
			item = this->create();
			item->setID(_id);

			if(item != NULL)
				this->add(item);
		}

		return item;
	}
	int deleteItem(string _id)
	{
		int index = 0;
		int size = this->size();

		for (int iloop = 0; iloop < size; iloop++)
		{
			BaseObject* baseObject = this->at(iloop);

			if (baseObject->getID().compare(_id) == 0)
			{
				index = iloop;
				break;
			}
		}

		this->erase(this->begin() + index);

		return 1;
	}
	virtual BaseObject* create() = 0;
};

class LibraryBase
{
private:
	ListOfBaseObject *_list;
protected:
	virtual ListOfBaseObject* createList() = 0;
	virtual void writeHeaderTo(ofstream *file) = 0;
	virtual void writeItemTo(BaseObject *item, ofstream *file) = 0;
public:
	LibraryBase()
	{
	}
	virtual ~LibraryBase()
	{

	}
	void setUp()
	{
		this->_list = this->createList();
		this->load();
	}
	virtual ListOfBaseObject* getList()
	{
		return _list;
	}
	virtual void writeTo(string fileName)
	{
		ofstream *file = new ofstream();

		file->open(fileName);

		ListOfBaseObject *list = this->getList();

		writeHeaderTo(file);

		if (list == NULL)
			return;

		int size = list->size();

		for (int iloop = 0; iloop < size; iloop++)
		{
			BaseObject *item = list->at(iloop);

			if (item == NULL)
				continue;

			writeItemTo(item, file);
		}

		file->close();
	}
	virtual void save() = 0;
	virtual void load() = 0;
};



class User : public BaseObject
{
public:
	string password;
	string fname;
	string lname;
	int userType;

	User()
	{
		this->password = "";
		this->fname = "";
		this->lname = "";
		this->userType = 0;
	}
	~User() override
	{

	}

	string getFullName()
	{
		return this->fname + " " + this->lname;
	}
	bool isadministrator()
	{
		if (this->userType == 1)
			return true;

		return false;
	}
};

class ListOfUser : public ListOfBaseObject
{
public:
	BaseObject* create() override
	{
		return new User();
	}
};

class LibraryUser : public LibraryBase
{
protected:
	ListOfBaseObject* createList() override
	{
		return new ListOfUser();
	}
	void writeHeaderTo(ofstream *file) override
	{
		*file << "-- \t ----- \t ----- \t ----\n";
		*file << "id \t fname \t lname \t type\n";
		*file << "-- \t ----- \t ----- \t ----\n";
	}
	void writeItemTo(BaseObject *item, ofstream *file) override
	{
		if (item == NULL)
			return;

		User *specificUser = (User *)item;

		if (specificUser == NULL)
			return;

		*file << specificUser->getID() << "\t" << specificUser->fname << "\t" << specificUser->lname << "\t" << specificUser->userType << "\n";
	}
public:
	LibraryUser()
	{
		
	}
	int Authenticate(string _uid, string _pwd)
	{
		User *user = (User *)this->getList()->find(_uid);
		int result = -1;

		if (user == NULL)
		{
			result = INVALID_USER;
		}
		else if (user->password != _pwd)
		{
			result = INVALID_PASSWORD;
		}
		else
		{
			result = SUCCESS;
		}

		return result;
	}
};

class LibraryUserFlatFile : public LibraryUser
{
protected:
	void load() override
	{
		//	We can make an API call to other system to fetch all the users.
		//	Currently we just hardcoded it. Later we will read it from file. 

		ifstream *myFile = new ifstream();
		string line;
		User *user = NULL;

		myFile->open(FILE_PATH + "users.txt");

		if (myFile->is_open())
		{
			while (myFile->eof() == false)
			{
				getline(*myFile, line);

				if (line.compare("#") == 0)
				{
					user = (User *)this->getList()->create();
					this->getList()->add(user);
				}
				else
				{
					if (user != NULL)
					{
						if (line.substr(0, 3).compare("id:") == 0)
						{
							user->setID(line.substr(3, line.length() - 3));
						}
						else if (line.substr(0, 6).compare("fname:") == 0)
						{
							user->fname = line.substr(3, line.length() - 6);
						}
						else if (line.substr(0, 6).compare("lname:") == 0)
						{
							user->lname = line.substr(3, line.length() - 6);
						}
						else if (line.substr(0, 9).compare("password:") == 0)
						{
							user->password = line.substr(9, line.length() - 9);
						}
						else if (line.substr(0, 5).compare("type:") == 0)
						{
							user->userType = stoi(line.substr(5, line.length() - 5));
						}
					}
				}
			}

			myFile->close();
		}
	}
	void save() override
	{

	}
public:
	LibraryUserFlatFile()
	{
		this->setUp();
	}
};

class LibraryUserDatabase : public LibraryUser
{
protected:
	void load() override
	{
		
	}
	void save() override
	{

	}
public:
	LibraryUserDatabase()
	{
		this->setUp();
	}
};

struct UserFactory
{
	static UserFactory* createFactory(char choice);
	virtual LibraryUser* createLibraryUser() = 0;
};

struct FlatFileUserFactory : UserFactory
{
	LibraryUser* createLibraryUser() override
	{
		return new LibraryUserFlatFile();
	}
};

struct DatabaseUserFactory : UserFactory
{
	LibraryUser* createLibraryUser() override
	{
		return new LibraryUserDatabase();
	}
};

UserFactory *UserFactory::createFactory(char choice)
{
	switch (choice)
	{
	case '1':	//	Flat file
		return new FlatFileUserFactory();
	case '2':	//	Database
		return new DatabaseUserFactory();
	case '3':	//	UMS Api
		break;
	}

	return new FlatFileUserFactory();
}



class Ticket : public BaseObject
{
protected:
	double datecreated;
	string details;
	int status;
public:
	Ticket()
	{
		datecreated = 0;
		details = "";
		status = 0;
	}
	~Ticket() override
	{

	}
	double getdatecreated()
	{
		return this->datecreated;
	}
	void setdatecreated(double dtcreated)
	{
		this->datecreated = dtcreated;
	}
	string getdetails()
	{
		return this->details;
	}
	void setdetails(string dtls)
	{
		this->details = dtls;
	}
	int getstatus()
	{
		return this->status;
	}
	void setstatus(int sts)
	{
		this->status = sts;
	}
};

class ListOfTicket : public ListOfBaseObject
{
public:
	BaseObject* create() override
	{
		return new Ticket();
	}
};

class LibraryTicket : public LibraryBase
{
protected:
	void writeHeaderTo(ofstream *file) override
	{
		*file << "-- \t ----- \t ------ \t -------\n";
		*file << "id \t dtcrt \t status \t details\n";
		*file << "-- \t ----- \t ------ \t -------\n";
	}
	void writeItemTo(BaseObject *item, ofstream *file) override
	{
		if (item == NULL)
			return;

		Ticket *ticket = (Ticket *)item;

		if (ticket == NULL)
			return;

		*file << ticket->getID() << "\t" << ticket->getdatecreated() << "\t" << ticket->getstatus() << "\t" << ticket->getdetails() << "\n";
	}
	ListOfBaseObject *createList() override
	{
		return new ListOfTicket();
	}
public:
	LibraryTicket()
	{
		
	}
};

class LibraryTicketFlatFile : public LibraryTicket
{
protected:
	void load() override
	{
		ifstream *myFile = new ifstream();
		string line;
		Ticket *ticket = NULL;

		myFile->open(FILE_PATH + "tickets.txt");

		if (myFile->is_open())
		{
			while (myFile->eof() == false)
			{
				getline(*myFile, line);

				if (line.compare("#") == 0)
				{
					ticket = (Ticket *)this->getList()->create();
					this->getList()->add(ticket);
				}
				else
				{
					if (ticket != NULL)
					{
						if (line.substr(0, 3).compare("id:") == 0)
						{
							ticket->setID(line.substr(3, line.length() - 3));
						}
						else if (line.substr(0, 10).compare("dtcreated:") == 0)
						{
							ticket->setdatecreated(stod(line.substr(10, line.length() - 10)));
						}
						else if (line.substr(0, 8).compare("details:") == 0)
						{
							ticket->setdetails(line.substr(8, line.length() - 8));
						}
						else if (line.substr(0, 7).compare("status:") == 0)
						{
							ticket->setstatus(stoi(line.substr(7, line.length() - 7)));
						}
					}
				}
			}

			myFile->close();
		}
	}
	void save() override
	{

	}
public:
	LibraryTicketFlatFile()
	{
		this->setUp();
	}
};

class LibraryTicketDatabase : public LibraryTicket
{
protected:
	void load() override
	{

	}
	void save() override
	{

	}
public:
	LibraryTicketDatabase()
	{
		this->setUp();
	}
};

struct TicketFactory
{
	static TicketFactory* createFactory(char choice);
	virtual LibraryTicket* createLibraryTicket() = 0;
};

struct FlatFileTicketFactory : TicketFactory
{
	LibraryTicket* createLibraryTicket() override
	{
		return new LibraryTicketFlatFile();
	}
};

struct DatabaseTicketFactory : TicketFactory
{
	LibraryTicket* createLibraryTicket() override
	{
		return new LibraryTicketDatabase();
	}
};

TicketFactory *TicketFactory::createFactory(char choice)
{
	switch (choice)
	{
	case '1':	//	Flat file
		return new FlatFileTicketFactory();
	case '2':	//	Database
		return new DatabaseTicketFactory();
		break;
	case '3':	//	UMS Api
		break;
	}

	return new FlatFileTicketFactory();
}




class Service : public BaseObject
{
private:
	double rateperunit;
	string description;
public:
	Service()
	{
		this->rateperunit = 0;
		this->description = "";
	}
	~Service() override
	{

	}
	double getrateperunit()
	{
		return this->rateperunit;
	}
	void setrateperunit(double rpu)
	{
		this->rateperunit = rpu;
	}
	string getdescription()
	{
		return this->description;
	}
	void setdescription(string desc)
	{
		this->description = desc;
	}
};

class ListOfService : public ListOfBaseObject
{
public:
	BaseObject* create() override
	{
		return new Service();
	}
};

class LibraryService : public LibraryBase
{
protected:
	void writeHeaderTo(ofstream *file) override
	{
		*file << "-- \t ----- \t ------ \t -------\n";
		*file << "id \t dtcrt \t status \t details\n";
		*file << "-- \t ----- \t ------ \t -------\n";
	}
	void writeItemTo(BaseObject *item, ofstream *file) override
	{
		if (item == NULL)
			return;

		Ticket *ticket = (Ticket *)item;

		if (ticket == NULL)
			return;

		*file << ticket->getID() << "\t" << ticket->getdatecreated() << "\t" << ticket->getstatus() << "\t" << ticket->getdetails() << "\n";
	}
	ListOfBaseObject *createList() override
	{
		return new ListOfTicket();
	}
public:
	LibraryService()
	{

	}
};

class LibraryServiceFlatFile : public LibraryService
{
protected:
	void load() override
	{
		ifstream *myFile = new ifstream();
		string line;
		Service *serviceSpecification = NULL;

		myFile->open(FILE_PATH + "servicespecifications.txt");

		if (myFile->is_open())
		{
			while (myFile->eof() == false)
			{
				getline(*myFile, line);

				if (line.compare("#") == 0)
				{
					serviceSpecification = (Service *)this->getList()->create();
					this->getList()->add(serviceSpecification);
				}
				else
				{
					if (serviceSpecification != NULL)
					{
						if (line.substr(0, 3).compare("id:") == 0)
						{
							serviceSpecification->setID(line.substr(3, line.length() - 3));
						}
						else if (line.substr(0, 12).compare("description:") == 0)
						{
							serviceSpecification->setdescription(line.substr(12, line.length() - 12));
						}
						else if (line.substr(0, 12).compare("rateperunit:") == 0)
						{
							serviceSpecification->setrateperunit(stod(line.substr(12, line.length() - 12)));
						}
					}
				}
			}
		}
	}
	void save() override
	{

	}

public:
	LibraryServiceFlatFile()
	{
		this->setUp();
	}
};

class LibraryServiceDatabase : public LibraryService
{
protected:
	void load() override
	{
		ifstream *myFile = new ifstream();
		string line;
		Service *serviceSpecification = NULL;

		myFile->open(FILE_PATH + "servicespecifications.txt");

		if (myFile->is_open())
		{
			while (myFile->eof() == false)
			{
				getline(*myFile, line);

				if (line.compare("#") == 0)
				{
					serviceSpecification = (Service *)this->getList()->create();
					this->getList()->add(serviceSpecification);
				}
				else
				{
					if (serviceSpecification != NULL)
					{
						if (line.substr(0, 3).compare("id:") == 0)
						{
							serviceSpecification->setID(line.substr(3, line.length() - 3));
						}
						else if (line.substr(0, 12).compare("description:") == 0)
						{
							serviceSpecification->setdescription(line.substr(12, line.length() - 12));
						}
						else if (line.substr(0, 12).compare("rateperunit:") == 0)
						{
							serviceSpecification->setrateperunit(stod(line.substr(12, line.length() - 12)));
						}
					}
				}
			}
		}
	}
	void save() override
	{

	}

public:
	LibraryServiceDatabase()
	{
		this->setUp();
	}
};

struct ServiceFactory
{
	static ServiceFactory* createFactory(char choice);
	virtual LibraryService* createLibraryService() = 0;
};

struct FlatFileServiceFactory : public ServiceFactory
{
	LibraryService* createLibraryService() override
	{
		return new LibraryServiceFlatFile();
	}
};

struct DatabaseServiceFactory : public ServiceFactory
{
	LibraryService* createLibraryService() override
	{
		return new LibraryServiceDatabase();
	}
};

ServiceFactory *ServiceFactory::createFactory(char choice)
{
	switch (choice)
	{
	case '1':	//	Flat file
		return new FlatFileServiceFactory();
	case '2':	//	Database
		return new DatabaseServiceFactory();
		break;
	case '3':	//	UMS Api
		break;
	}

	return new FlatFileServiceFactory();
}



class ServicePerformed : public BaseObject
{
protected:
	int clockin;
	int clockout;
	string notes;
	string servicetypeid;
public:

	ServicePerformed()
	{
		this->clockin = 0;
		this->clockout = 0;
		this->id = "";
		this->notes = "";
		this->servicetypeid = "";
	}
	ServicePerformed(string _servicetypeid)
	{
		this->clockin = 0;
		this->clockout = 0;
		this->id = "";
		this->notes = "";
		this->servicetypeid = _servicetypeid;
	}
	~ServicePerformed() override
	{

	}

	int getclockin()
	{
		return this->clockin;
	}
	void setclockin(int clkin)
	{
		this->clockin = clkin;
	}
	int getclockout()
	{
		return this->clockout;
	}
	void setclockout(int clkout)
	{
		this->clockout = clkout;
	}
	string getnotes()
	{
		return this->notes;
	}
	void setnotes(string nts)
	{
		this->notes = nts;
	}
	string getservicetypeid()
	{
		return this->servicetypeid;
	}
	void setservicetypeid(string nts)
	{
		this->servicetypeid = nts;
	}
	double getTimeDifference()
	{
		double a = this->clockout - this->clockin - 40;

		if (a < 0)
			a = 0;

		return a;
	}
	virtual double calculate(Service *serviceSpecification)
	{
		if (serviceSpecification != NULL)
			return this->getTimeDifference() * serviceSpecification->getrateperunit();
		else
			return 0;		
	}
};

class ListOfServicePerformed : public ListOfBaseObject
{
private:
	string generateID(string _servicetypeid, int _clockintime)
	{
		return _servicetypeid + "_" + to_string(_clockintime);
	}
public:
	BaseObject* create() override
	{
		return new ServicePerformed();
	}
	int clockIn(string _servicetypeid, int _clockintime)
	{
		string id = generateID(_servicetypeid, _clockintime);
		ServicePerformed *newServicePerformed = (ServicePerformed *)this->find(id);

		if (newServicePerformed == NULL)
		{
			newServicePerformed = new ServicePerformed(_servicetypeid);
			newServicePerformed->setID(generateID(_servicetypeid, _clockintime));
			this->add(newServicePerformed);
		}

		if (newServicePerformed != NULL)
			newServicePerformed->setclockin(_clockintime);

		return SUCCESS;
	}
	int updateNotes(string _serviceperformedid, string notes)
	{
		ServicePerformed *serviceperformed = (ServicePerformed *)this->find(_serviceperformedid);

		if (serviceperformed == NULL)
			return INVALID_SERVICEID;

		string nts = serviceperformed->getnotes();
		nts += notes;
		serviceperformed->setnotes(nts);

		return SUCCESS;
	}
	int deleteService(string _serviceperformedid)
	{
		ServicePerformed *serviceperformed = (ServicePerformed *)this->find(_serviceperformedid);

		if (serviceperformed == NULL)
			return INVALID_SERVICEID;

		this->deleteItem(_serviceperformedid);

		return SUCCESS;
	}
	int clockOut(string _serviceperformedid, int _clockouttime)
	{
		ServicePerformed *serviceperformed = (ServicePerformed *)this->find(_serviceperformedid);

		if (serviceperformed == NULL)
			return INVALID_SERVICEID;

		if (_clockouttime < serviceperformed->getclockin())
			return INVALID_CLOCKOUTTIME;

		serviceperformed->setclockout(_clockouttime);

		return SUCCESS;
	}
};




class ServicesPerformedOnTicket : public BaseObject
{
private:
	string userid;
	string ticketid;
	ListOfServicePerformed *servicePerformedList;
public:
	string getuserid()
	{
		return this->userid;
	}
	void setuserid(string uid)
	{
		this->userid = uid;
	}
	string getticketid()
	{
		return this->ticketid;
	}
	void setticketid(string tckid)
	{
		this->ticketid = tckid;
	}
	ListOfServicePerformed* getserviceperformedlist()
	{
		return this->servicePerformedList;
	}
	void setserviceperformedlist(ListOfServicePerformed *lstserviceperformed)
	{
		this->servicePerformedList = lstserviceperformed;
	}

	ServicesPerformedOnTicket()
	{
		this->userid = "";
		this->servicePerformedList = new ListOfServicePerformed();
		this->ticketid = "";
	}
	~ServicesPerformedOnTicket() override
	{

	}
};

class ListOfServicesPerformedOnTicket : public ListOfBaseObject
{
private:
	string generateID(string _ticketID, string _userID)
	{
		return _ticketID + "_" + _userID;
	}
public:
	ListOfServicesPerformedOnTicket()
	{

	}
	BaseObject* create() override
	{
		return new ServicesPerformedOnTicket();
	}
	BaseObject* find(string _ticketid, string _userid)
	{
		string id = generateID(_ticketid, _userid);
		BaseObject *item = ((ListOfBaseObject *)this)->find(id);

		return item;
	}
	BaseObject* get(string _ticketid, string _userid)
	{
		string id = generateID(_ticketid, _userid);

		BaseObject *item = ((ListOfBaseObject *)this)->get(id);

		if (item != NULL)
		{
			if(((ServicesPerformedOnTicket *)item)->getticketid().compare("") == 0)
				((ServicesPerformedOnTicket *)item)->setticketid(_ticketid);

			if (((ServicesPerformedOnTicket *)item)->getuserid().compare("") == 0)
				((ServicesPerformedOnTicket *)item)->setuserid(_userid);
		}

		return item;
	}
	int assignTicket(string ticketID, string assignedUserID)
	{
		BaseObject *item = this->get(ticketID, assignedUserID);

		return item != NULL ? SUCCESS : SYSTEM_ERROR;
	}
};

class LibraryServicesPerformedOnTicket : public LibraryBase
{
protected:
	ListOfBaseObject *createList() override
	{
		return new ListOfServicesPerformedOnTicket();
	}
	void writeHeaderTo(ofstream *file) override
	{

	}
	void writeItemTo(BaseObject *item, ofstream *file) override
	{

	}
public:
	LibraryServicesPerformedOnTicket()
	{
		
	}	
};

class LibraryServicePerformedOnTicketFlatFile : public LibraryServicesPerformedOnTicket
{
protected:
	void load() override
	{
		ifstream *myFile = new ifstream();
		string line;
		Ticket *ticket = NULL;
		ServicesPerformedOnTicket *ts = NULL;
		ServicePerformed *serviceperformed = NULL;

		myFile->open(FILE_PATH + "ticketservicesperformed.txt");

		if (myFile->is_open())
		{
			while (myFile->eof() == false)
			{
				getline(*myFile, line);

				if (line.compare("#") == 0)
				{
					ts = new ServicesPerformedOnTicket();
					this->getList()->add(ts);
				}
				else if (line.compare("#1") == 0)
				{
					serviceperformed = new ServicePerformed("1");
					if (ts != NULL)
						ts->getserviceperformedlist()->add(serviceperformed);
				}
				else if (line.compare("#2") == 0)
				{
					serviceperformed = new ServicePerformed("2");
					if (ts != NULL)
						ts->getserviceperformedlist()->add(serviceperformed);
				}
				else if (line.compare("#3") == 0)
				{
					serviceperformed = new ServicePerformed("3");
					if (ts != NULL)
						ts->getserviceperformedlist()->add(serviceperformed);
				}
				else if (line.compare("#4") == 0)
				{
					serviceperformed = new ServicePerformed("4");
					if (ts != NULL)
						ts->getserviceperformedlist()->add(serviceperformed);
				}
				else if (line.compare("#5") == 0)
				{
					serviceperformed = new ServicePerformed("5");
					if (ts != NULL)
						ts->getserviceperformedlist()->add(serviceperformed);
				}
				else if (line.substr(0, 3).compare("id:") == 0)
				{
					if (ts != NULL)
						ts->setID(line.substr(3, line.length() - 3));
				}
				else if (line.substr(0, 9).compare("ticketid:") == 0)
				{
					if (ts != NULL)
						ts->setticketid(line.substr(9, line.length() - 9));
				}
				else if (line.substr(0, 7).compare("userid:") == 0)
				{
					if (ts != NULL)
						ts->setuserid(line.substr(7, line.length() - 7));
				}
				else if (line.substr(0, 10).compare("serviceid:") == 0)
				{
					if (serviceperformed != NULL)
						serviceperformed->setID(line.substr(10, line.length() - 10));
				}
				else if (line.substr(0, 8).compare("clockin:") == 0)
				{
					if (serviceperformed != NULL)
						serviceperformed->setclockin(stoi(line.substr(8, line.length() - 8)));
				}
				else if (line.substr(0, 9).compare("clockout:") == 0)
				{
					if (serviceperformed != NULL)
						serviceperformed->setclockout(stoi(line.substr(9, line.length() - 9)));
				}
				else if (line.substr(0, 6).compare("notes:") == 0)
				{
					if (serviceperformed != NULL)
						serviceperformed->setnotes(line.substr(6, line.length() - 6));
				}
			}
		}
	}
	void save() override
	{
		ofstream *file = new ofstream();

		file->open(FILE_PATH + "ticketservicesperformed.txt");

		if (file->is_open())
		{
			int size = this->getList()->size();

			for (int iloop = 0; iloop < size; iloop++)
			{
				ServicesPerformedOnTicket *ts = (ServicesPerformedOnTicket *)this->getList()->at(iloop);

				*file << "#" << "\n";
				*file << "id:" << ts->getID() << "\n";
				*file << "ticketid:" << ts->getticketid() << "\n";
				*file << "userid:" << ts->getuserid() << "\n";

				int size1 = ts->getserviceperformedlist()->size();

				for (int jloop = 0; jloop < size1; jloop++)
				{
					ServicePerformed *s = (ServicePerformed *)ts->getserviceperformedlist()->at(jloop);

					*file << "#" << s->getservicetypeid() << "\n";
					*file << "serviceid:" << s->getID() << "\n";
					*file << "clockin:" << s->getclockin() << "\n";
					*file << "clockout:" << s->getclockout() << "\n";
					*file << "notes:" << s->getnotes() << "\n";
				}
			}

			file->close();
		}
	}

public:
	LibraryServicePerformedOnTicketFlatFile()
	{
		this->setUp();
	}
};

class LibraryServicePerformedOnTicketDatabase : public LibraryServicesPerformedOnTicket
{
protected:
	void load() override
	{
		ifstream *myFile = new ifstream();
		string line;
		Ticket *ticket = NULL;
		ServicesPerformedOnTicket *ts = NULL;
		ServicePerformed *serviceperformed = NULL;

		myFile->open(FILE_PATH + "ticketservicesperformed.txt");

		if (myFile->is_open())
		{
			while (myFile->eof() == false)
			{
				getline(*myFile, line);

				if (line.compare("#") == 0)
				{
					ts = new ServicesPerformedOnTicket();
					this->getList()->add(ts);
				}
				else if (line.compare("#1") == 0)
				{
					serviceperformed = new ServicePerformed("1");
					if (ts != NULL)
						ts->getserviceperformedlist()->add(serviceperformed);
				}
				else if (line.compare("#2") == 0)
				{
					serviceperformed = new ServicePerformed("2");
					if (ts != NULL)
						ts->getserviceperformedlist()->add(serviceperformed);
				}
				else if (line.compare("#3") == 0)
				{
					serviceperformed = new ServicePerformed("3");
					if (ts != NULL)
						ts->getserviceperformedlist()->add(serviceperformed);
				}
				else if (line.compare("#4") == 0)
				{
					serviceperformed = new ServicePerformed("4");
					if (ts != NULL)
						ts->getserviceperformedlist()->add(serviceperformed);
				}
				else if (line.compare("#5") == 0)
				{
					serviceperformed = new ServicePerformed("5");
					if (ts != NULL)
						ts->getserviceperformedlist()->add(serviceperformed);
				}
				else if (line.substr(0, 3).compare("id:") == 0)
				{
					if (ts != NULL)
						ts->setID(line.substr(3, line.length() - 3));
				}
				else if (line.substr(0, 9).compare("ticketid:") == 0)
				{
					if (ts != NULL)
						ts->setticketid(line.substr(9, line.length() - 9));
				}
				else if (line.substr(0, 7).compare("userid:") == 0)
				{
					if (ts != NULL)
						ts->setuserid(line.substr(7, line.length() - 7));
				}
				else if (line.substr(0, 10).compare("serviceid:") == 0)
				{
					if (serviceperformed != NULL)
						serviceperformed->setID(line.substr(10, line.length() - 10));
				}
				else if (line.substr(0, 8).compare("clockin:") == 0)
				{
					if (serviceperformed != NULL)
						serviceperformed->setclockin(stoi(line.substr(8, line.length() - 8)));
				}
				else if (line.substr(0, 9).compare("clockout:") == 0)
				{
					if (serviceperformed != NULL)
						serviceperformed->setclockout(stoi(line.substr(9, line.length() - 9)));
				}
				else if (line.substr(0, 6).compare("notes:") == 0)
				{
					if (serviceperformed != NULL)
						serviceperformed->setnotes(line.substr(6, line.length() - 6));
				}
			}
		}
	}
	void save() override
	{
		ofstream *file = new ofstream();

		file->open(FILE_PATH + "ticketservicesperformed.txt");

		if (file->is_open())
		{
			int size = this->getList()->size();

			for (int iloop = 0; iloop < size; iloop++)
			{
				ServicesPerformedOnTicket *ts = (ServicesPerformedOnTicket *)this->getList()->at(iloop);

				*file << "#" << "\n";
				*file << "id:" << ts->getID() << "\n";
				*file << "ticketid:" << ts->getticketid() << "\n";
				*file << "userid:" << ts->getuserid() << "\n";

				int size1 = ts->getserviceperformedlist()->size();

				for (int jloop = 0; jloop < size1; jloop++)
				{
					ServicePerformed *s = (ServicePerformed *)ts->getserviceperformedlist()->at(jloop);

					*file << "#" << s->getservicetypeid() << "\n";
					*file << "serviceid:" << s->getID() << "\n";
					*file << "clockin:" << s->getclockin() << "\n";
					*file << "clockout:" << s->getclockout() << "\n";
					*file << "notes:" << s->getnotes() << "\n";
				}
			}

			file->close();
		}
	}


public:
	LibraryServicePerformedOnTicketDatabase()
	{
		this->setUp();
	}
};

struct ServicePerformedOnTicketFactory
{
	static ServicePerformedOnTicketFactory* createFactory(char choice);
	virtual LibraryServicesPerformedOnTicket* createLibraryServicePerformed() = 0;
};

struct FlatFileServicePerformedOnTicketFactory : public ServicePerformedOnTicketFactory
{
	LibraryServicesPerformedOnTicket* createLibraryServicePerformed() override
	{
		return new LibraryServicePerformedOnTicketFlatFile();
	}
};

struct DatabaseServicePerformedOnTicketFactory : public ServicePerformedOnTicketFactory
{
	LibraryServicesPerformedOnTicket* createLibraryServicePerformed() override
	{
		return new LibraryServicePerformedOnTicketDatabase();
	}
};

ServicePerformedOnTicketFactory *ServicePerformedOnTicketFactory::createFactory(char choice)
{	
	switch (choice)
	{
	case '1':	//	Flat file
		return new FlatFileServicePerformedOnTicketFactory();
	case '2':	//	Database
		return new DatabaseServicePerformedOnTicketFactory();
		break;
	case '3':	//	UMS Api
		break;
	}

	return new FlatFileServicePerformedOnTicketFactory();
}


class ConfigReader
{
private:
	char userChoice;
	char ticketChoice;
	char serviceChoice;
	char servicePerformedChoice;
public:
	char getUserChoice()
	{
		return this->userChoice;
	}
	char getTicketChoice()
	{
		return this->ticketChoice;
	}
	char getServiceChoice()
	{
		return this->serviceChoice;
	}
	char getServicePerformedChoice()
	{
		return this->servicePerformedChoice;
	}
	void readConfig()
	{
		ifstream *myFile = new ifstream();
		string line;
		string option;

		myFile->open(FILE_PATH + "config.txt");

		if (myFile->is_open())
		{
			while (myFile->eof() == false)
			{
				getline(*myFile, line);

				if (line.substr(0, 17).compare("serviceperformed:") == 0)
				{
					this->servicePerformedChoice = line.substr(17, line.length() - 17)[0];
				}
				else if (line.substr(0, 9).compare("services:") == 0)
				{
					this->serviceChoice = line.substr(9, line.length() - 9)[0];
				}
				else if (line.substr(0, 7).compare("ticket:") == 0)
				{
					this->ticketChoice = line.substr(7, line.length() - 7)[0];
				}
				else if (line.substr(0, 5).compare("user:") == 0)
				{
					this->userChoice = line.substr(5, line.length() - 5)[0];
				}
			}
			myFile->close();
		}
	}
};


class ServiceTrackerApplication
{
private:
	LibraryUser *_usersLibary;
	LibraryTicket *_ticketsLibrary;
	LibraryService *_serviceLibrary;
	LibraryServicesPerformedOnTicket *_servicesPerformedOnTicketLibrary;
	ConfigReader  *_configReader;
	User *_loggedInuser;
protected:
	LibraryUser* getLibraryUser()
	{
		if (this->_usersLibary == NULL)
		{
			UserFactory *theUserFactory = UserFactory::createFactory(this->getConfigReader()->getUserChoice());
			this->_usersLibary = theUserFactory->createLibraryUser();
		}

		return this->_usersLibary;
	}
	LibraryTicket* getLibraryTicket()
	{
		if (this->_ticketsLibrary == NULL)
		{
			TicketFactory *theTicketFactory = TicketFactory::createFactory(this->getConfigReader()->getTicketChoice());
			this->_ticketsLibrary = theTicketFactory->createLibraryTicket();
		}

		return this->_ticketsLibrary;
	}
	LibraryService* getLibraryService()
	{
		if (this->_serviceLibrary == NULL)
		{
			ServiceFactory *theServiceFactory = ServiceFactory::createFactory(this->getConfigReader()->getServiceChoice());
			this->_serviceLibrary = theServiceFactory->createLibraryService();
		}

		return this->_serviceLibrary;
	}
	LibraryServicesPerformedOnTicket* getLibraryServicesPerformedOnTicket()
	{
		if (this->_servicesPerformedOnTicketLibrary == NULL)
		{
			ServicePerformedOnTicketFactory *theServicePerformedOnTicketFactory = ServicePerformedOnTicketFactory::createFactory(this->getConfigReader()->getServicePerformedChoice());
			this->_servicesPerformedOnTicketLibrary = theServicePerformedOnTicketFactory->createLibraryServicePerformed();
		}
		return this->_servicesPerformedOnTicketLibrary;
	}
	ConfigReader* getConfigReader()
	{
		if (this->_configReader == NULL)
		{
			this->_configReader = new ConfigReader();
			this->_configReader->readConfig();
		}

		return this->_configReader;
	}
public:
	ServiceTrackerApplication()
	{
		this->_usersLibary = NULL;
		this->_ticketsLibrary = NULL;
		this->_serviceLibrary = NULL;
		this->_servicesPerformedOnTicketLibrary = NULL;
		this->_configReader = NULL;
	}
	~ServiceTrackerApplication()
	{

	}
	int authenticateUser(string _userID, string _pwd)
	{
		return this->getLibraryUser()->Authenticate(_userID, _pwd);
	}
	int setLoggedInUser(string _userID)
	{
		this->_loggedInuser = (User *)this->getLibraryUser()->getList()->find(_userID);

		if (this->_loggedInuser != NULL)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	string getLoggedInUserName()
	{
		if (this->_loggedInuser == NULL)
			return "";

		return this->_loggedInuser->getFullName();
	}
	bool isloggedInUserAdministrator()
	{
		if (this->_loggedInuser == NULL)
			return false;

		return this->_loggedInuser->isadministrator();
	}
	int assignTicket(string _ticketid, string _assigneduserid)
	{
		if(!this->isTicketValid(_ticketid))
			return INVALID_TICKET;

		if(!this->isValidUser(_assigneduserid))
			return INVALID_USER;

		if (this->_loggedInuser == NULL || !this->isloggedInUserAdministrator())
			return NO_PERMISSION;

		int ret = ((ListOfServicesPerformedOnTicket *)this->getLibraryServicesPerformedOnTicket()->getList())->assignTicket(_ticketid, _assigneduserid);

		return ret;
	}
	bool isValidUser(string _userID)
	{
		return this->getLibraryUser()->getList()->isexits(_userID);
	}
	bool isTicketAssigned(string _ticketid)
	{
		ServicesPerformedOnTicket *item = this->getServicesPerformedForTicketByCurrentUser(_ticketid);

		return (item != NULL);
	}
	ServicesPerformedOnTicket* getServicesPerformedForTicketByCurrentUser(string _ticketid)
	{
		ServicesPerformedOnTicket *item = (ServicesPerformedOnTicket *)((ListOfServicesPerformedOnTicket *)this->getLibraryServicesPerformedOnTicket()->getList())->find(_ticketid, this->_loggedInuser->getID());
		
		return item;
	}
	bool isTicketValid(string _ticketid)
	{
		return this->getLibraryTicket()->getList()->isexits(_ticketid);
	}
	bool isValidServiceType(string _serviceType)
	{
		return this->getLibraryService()->getList()->isexits(_serviceType);
	}
	bool isValidServiceForTicket(string _ticketid, string _serviceid)
	{
		ServicesPerformedOnTicket* item = (ServicesPerformedOnTicket *)((ListOfServicesPerformedOnTicket *)this->getLibraryServicesPerformedOnTicket()->getList())->find(_ticketid, this->_loggedInuser->getID());

		if (item != NULL)
		{
			BaseObject *s = item->getserviceperformedlist()->find(_serviceid);

			return s != NULL;
		}

		return false;
	}
	int clockInService(string _ticketid, string _servicetypeid, int _clockintime)
	{
		if (!this->isTicketValid(_ticketid))
			return INVALID_TICKET;

		if (!this->isTicketAssigned(_ticketid))
			return TICKET_NOT_ASSIGNED;

		if (!this->isValidServiceType(_servicetypeid))
			return INVALID_SERVICETYPE;

		if (_clockintime < 0 || _clockintime > 2400)
			return INVALID_CLOCKINTIME;

		return this->getServicesPerformedForTicketByCurrentUser(_ticketid)->getserviceperformedlist()->clockIn(_servicetypeid, _clockintime);
	}
	int clockOutService(string _ticketid, string _serviceid, int _clockouttime)
	{
		if (!this->isTicketValid(_ticketid))
			return INVALID_TICKET;

		if (!this->isTicketAssigned(_ticketid))
			return TICKET_NOT_ASSIGNED;

		if (_clockouttime < 0 || _clockouttime > 2400)
			return INVALID_CLOCKOUTTIME;

		return this->getServicesPerformedForTicketByCurrentUser(_ticketid)->getserviceperformedlist()->clockOut(_serviceid, _clockouttime);
	}
	int appendNotesToService(string _ticketid, string _serviceid, string _notes)
	{
		if (!this->isTicketValid(_ticketid))
			return INVALID_TICKET;

		if (!this->isTicketAssigned(_ticketid))
			return TICKET_NOT_ASSIGNED;

		return this->getServicesPerformedForTicketByCurrentUser(_ticketid)->getserviceperformedlist()->updateNotes(_serviceid, _notes);
	}
	int deleteService(string _ticketid, string _serviceid)
	{
		if (!this->isTicketValid(_ticketid))
			return INVALID_TICKET;

		if (!this->isTicketAssigned(_ticketid))
			return TICKET_NOT_ASSIGNED;

		return this->getServicesPerformedForTicketByCurrentUser(_ticketid)->getserviceperformedlist()->deleteItem(_serviceid);
	}
	void writeUsersTo(string fileName)
	{
		this->getLibraryUser()->writeTo(fileName);
	}
	void writeTicketsTo(string fileName)
	{
		this->getLibraryTicket()->writeTo(fileName);
	}
	void writeServicesPerformedTo(string fileName)
	{
		ofstream *file = new ofstream();

		file->open(fileName);

		if (file->is_open())
		{
			*file << "---------\t ------\t ------\t ----\t -----\t ------\t ---\t -----\n";
			*file << "Ticket ID\t UserID\t ServID\t Type\t ClkIn\t ClkOut\t Amt\t Notes\n";
			*file << "---------\t ------\t ------\t ----\t -----\t ------\t ---\t -----\n";

			int length = this->getLibraryServicesPerformedOnTicket()->getList()->size();
			double total = 0;
			double temp1 = 0;
			int temp;

			for (int iloop = 0; iloop < length; iloop++)
			{
				ServicesPerformedOnTicket *servicesperformedforticketbyuser = (ServicesPerformedOnTicket *)this->getLibraryServicesPerformedOnTicket()->getList()->at(iloop);

				temp = servicesperformedforticketbyuser->getserviceperformedlist()->size();

				for (int jloop = 0; jloop < temp; jloop++)
				{
					ServicePerformed *serviceperformed = (ServicePerformed *)servicesperformedforticketbyuser->getserviceperformedlist()->at(jloop);

					temp1 = serviceperformed->calculate((Service *)this->getLibraryService()->getList()->find(serviceperformed->getservicetypeid()));
					total += temp1;

					*file << servicesperformedforticketbyuser->getticketid() << "\t" << servicesperformedforticketbyuser->getuserid() << "\t" << serviceperformed->getID() << "\t" << serviceperformed->getservicetypeid() << "\t" << serviceperformed->getclockin() << "\t" << serviceperformed->getclockout() << "\t" << temp1 << "\t" << serviceperformed->getnotes() << "\n";
				}
			}
			*file << "-------------------------------------------------------------------------\n";
			*file << "Total : " << total << "\n";
			*file << "-------------------------------------------------------------------------\n";
		}

		file->close();
	}
	void writeServicesPerformedOnTicketIDTo(string ticketid, string fileName)
	{
		ofstream *file = new ofstream();

		file->open(fileName);

		if (file->is_open())
		{
			*file << "---------\t ------\t ------\t ----\t -----\t ------\t ---\t -----\n";
			*file << "Ticket ID\t UserID\t ServID\t Type\t ClkIn\t ClkOut\t Amt\t Notes\n";
			*file << "---------\t ------\t ------\t ----\t -----\t ------\t ---\t -----\n";

			int length = this->getLibraryServicesPerformedOnTicket()->getList()->size();
			double total = 0;
			double temp1 = 0;
			int temp;

			for (int iloop = 0; iloop < length; iloop++)
			{
				ServicesPerformedOnTicket *servicesperformedforticketbyuser = (ServicesPerformedOnTicket *)this->getLibraryServicesPerformedOnTicket()->getList()->at(iloop);

				if (servicesperformedforticketbyuser->getticketid() == ticketid)
				{
					temp = servicesperformedforticketbyuser->getserviceperformedlist()->size();

					for (int jloop = 0; jloop < temp; jloop++)
					{
						ServicePerformed *serviceperformed = (ServicePerformed *)servicesperformedforticketbyuser->getserviceperformedlist()->at(jloop);

						temp1 = serviceperformed->calculate((Service *)this->getLibraryService()->getList()->find(serviceperformed->getservicetypeid()));
						total += temp1;

						*file << servicesperformedforticketbyuser->getticketid() << "\t" << servicesperformedforticketbyuser->getuserid() << "\t" << serviceperformed->getID() << "\t" << serviceperformed->getservicetypeid() << "\t" << serviceperformed->getclockin() << "\t" << serviceperformed->getclockout() << "\t" << temp << "\t" << serviceperformed->getnotes() << "\n";
					}
				}
			}
			*file << "-------------------------------------------------------------------------\n";
			*file << "Total : " << total << "\n";
			*file << "-------------------------------------------------------------------------\n";
		}

		file->close();
	}
	void writeServicesPerformedOnUserIDTo(string userid, string fileName)
	{
		ofstream *file = new ofstream();

		file->open(fileName);

		if (file->is_open())
		{
			*file << "---------\t ------\t ------\t ----\t -----\t ------\t ---\t -----\n";
			*file << "Ticket ID\t UserID\t ServID\t Type\t ClkIn\t ClkOut\t Amt\t Notes\n";
			*file << "---------\t ------\t ------\t ----\t -----\t ------\t ---\t -----\n";

			int length = this->getLibraryServicesPerformedOnTicket()->getList()->size();
			double total = 0;
			double temp1 = 0;
			int temp = 0;

			for (int iloop = 0; iloop < length; iloop++)
			{
				ServicesPerformedOnTicket *servicesperformedforticketbyuser = (ServicesPerformedOnTicket *)this->getLibraryServicesPerformedOnTicket()->getList()->at(iloop);

				if (servicesperformedforticketbyuser->getuserid() == userid)
				{
					temp = servicesperformedforticketbyuser->getserviceperformedlist()->size();

					for (int jloop = 0; jloop < temp; jloop++)
					{
						ServicePerformed *serviceperformed = (ServicePerformed *)servicesperformedforticketbyuser->getserviceperformedlist()->at(jloop);

						temp1 = serviceperformed->calculate((Service *)this->getLibraryService()->getList()->find(serviceperformed->getservicetypeid()));
						total += temp1;

						*file << servicesperformedforticketbyuser->getticketid() << "\t" << servicesperformedforticketbyuser->getuserid() << "\t" << serviceperformed->getID() << "\t" << serviceperformed->getservicetypeid() << "\t" << serviceperformed->getclockin() << "\t" << serviceperformed->getclockout() << "\t" << temp1 << "\t" << serviceperformed->getnotes() << "\n";
					}
				}
			}
			*file << "-------------------------------------------------------------------------\n";
			*file << "Total : " << total << "\n";
			*file << "-------------------------------------------------------------------------\n";
		}

		file->close();
	}
	void writeAssignmentsTo(string fileName)
	{
		ofstream *file = new ofstream();

		file->open(fileName);

		if (file->is_open())
		{
			*file << "---------\t ------\n";
			*file << "Ticket ID\t UserID\n";
			*file << "---------\t ------\n";

			int length = this->getLibraryServicesPerformedOnTicket()->getList()->size();

			for (int iloop = 0; iloop < length; iloop++)
			{
				ServicesPerformedOnTicket *servicesperformedforticketbyuser = (ServicesPerformedOnTicket *)this->getLibraryServicesPerformedOnTicket()->getList()->at(iloop);

				*file << servicesperformedforticketbyuser->getticketid() << "\t" << servicesperformedforticketbyuser->getuserid() << "\n";
			}
		}

		file->close();
	}
	void saveticketservices()
	{
		this->getLibraryServicesPerformedOnTicket()->save();
	}
};

class TestUI
{
private:
	string printFileName = FILE_PATH + "print.txt";
public:
	ServiceTrackerApplication* _tracker;


	TestUI(ServiceTrackerApplication* _servTrac)
	{
		_tracker = _servTrac;
	}

	void showUI()
	{
		string userID = "";
		string password = "";
		int failedAttempt = 0;		

		while (true)
		{
			if (failedAttempt == -1)
			{
				system("PAUSE");
				break;
			}
			else if (failedAttempt == 3)
			{
				cout << "\n3 failed attempts. System will quit !!!";
				break;
			}
			else
			{
				userID = inputFromUser("user id");
				password = inputFromUser("passowrd");

				int ret = _tracker->authenticateUser(userID, password);

				if (ret == INVALID_USER)
				{
					cout << "\nUser not valid !!!";
					failedAttempt++;
				}
				else if (ret == INVALID_PASSWORD)
				{
					cout << "\nPassowrd incorrect !!!";
					failedAttempt++;
				}
				else
				{
					ret = _tracker->setLoggedInUser(userID);

					if (ret != SUCCESS)
					{
						processMessage(ret);
						failedAttempt++;
					}
					else
					{
						cout << "\nYay got it!!!\n";

						failedAttempt = 0;

						system("PAUSE");
						system("CLS");

						showMenu(_tracker->getLoggedInUserName());

						if (performOptions() == -1)
							failedAttempt = -1;

					}
				}
			}
		}

		system("PAUSE");
	}
	void printtoscreen()
	{
		ifstream *file = new ifstream();
		string line = "";
		file->open(printFileName);

		if (file->is_open())
		{
			while (file->eof() == false)
			{
				getline(*file, line);

				cout << line << "\n";
			}
		}

		file->close();
	}
	int performOptions()
	{
		string option = "";
		int returnVal = 0;

		while (true)
		{
			cout << "\n=============================================================================================";
			option = inputFromUser("Option");

			if (option.compare("1") == 0)
			{
				processAssignTicket(_tracker);
			}
			else if (option.compare("2") == 0)
			{
				processClockIn(_tracker);
			}
			else if (option.compare("3") == 0)
			{
				processClockOut(_tracker);
			}
			else if (option.compare("4") == 0)
			{
				processUpdateNotes(_tracker);
			}
			else if (option.compare("5") == 0)
			{
				processDelete(_tracker);
			}
			else if (option.compare("6a") == 0)
			{
				_tracker->writeUsersTo(printFileName);
				printtoscreen();
			}
			else if (option.compare("6b") == 0)
			{
				_tracker->writeTicketsTo(printFileName);
				printtoscreen();
			}
			else if (option.compare("6c") == 0)
			{
				_tracker->writeAssignmentsTo(printFileName);
				printtoscreen();
			}
			else if (option.compare("6d") == 0)
			{
				processServiceReportBasedOnTicketID(_tracker);
			}
			else if (option.compare("6e") == 0)
			{
				processServiceReportBasedOnUserID(_tracker);
			}
			else if (option.compare("6f") == 0)
			{
				_tracker->writeServicesPerformedTo(printFileName);
				printtoscreen();
			}
			else if (option.compare("7") == 0)
			{
				system("CLS");
				break;
			}
			else if (option.compare("8") == 0)
			{
				system("CLS");
				showMenu(_tracker->getLoggedInUserName());
			}
			else if (option.compare("9") == 0)
			{
				showMenu(_tracker->getLoggedInUserName());
			}
			else if (option.compare("10") == 0)
			{
				//	Quit
				returnVal = -1;
				break;
			}
			else
			{
				cout << "\nInvalid Option !!!\n";
			}

			system("PAUSE");
		}

		return returnVal;
	}
	void processMessage(int messageID)
	{
		if (messageID == INVALID_USER)
			cout << "\nInvalid User !!!\n";
		else if (messageID == INVALID_PASSWORD)
			cout << "\nInvalid Password !!!\n";
		else if (messageID == INVALID_TICKET)
			cout << "\nInvalid Ticket !!!\n";
		else if (messageID == INVALID_SERVICETYPE)
			cout << "\nInvalid Service Type !!!\n";
		else if (messageID == INVALID_SERVICEID)
			cout << "\nInvalid Service ID !!!\n";
		else if (messageID == INVALID_CLOCKINTIME)
			cout << "\nInvalid Clock In Time !!!\n";
		else if (messageID == INVALID_CLOCKOUTTIME)
			cout << "\nInvalid Clock Out Time !!!\n";
		else if (messageID == NO_PERMISSION)
			cout << "\nUser doesn't have permission !!!\n";
		else if (messageID == TICKET_NOT_ASSIGNED)
			cout << "\nTicket not assigned to the user !!!\n";
		else if (messageID == SYSTEM_ERROR)
			cout << "\nSystem error occurred.\n";
		else
			cout << "\nOperation successfull !!!\n";
	}
	void showMenu(string username)
	{
		cout << "*********************************************************************************************" << "\n";
		cout << "                                 SERVICE TRACKER " << username << "\n";
		cout << "*********************************************************************************************" << "\n";
		cout << "1 => Assign Ticket." << "\n";
		cout << "2 => Clock In Service." << "\n";
		cout << "3 => Clock Out Service." << "\n";
		cout << "4 => Update Notes In Service." << "\n";
		cout << "5 => Delete Service." << "\n";

		cout << "6 => Report." << "\n";

		cout << "\ta) User." << "\n";
		cout << "\tb) Tickets." << "\n";
		cout << "\tc) Assignment." << "\n";
		cout << "\td) Service based on ticket id." << "\n";
		cout << "\te) Service based on user id." << "\n";
		cout << "\tf) All services." << "\n";


		cout << "7 => Log Out." << "\n";
		cout << "8 => Clear Screen." << "\n";
		cout << "9 => Show Menu." << "\n";
		cout << "10 => Quit." << "\n";
		cout << "*********************************************************************************************" << "\n";
	}
	string inputFromUser(string question)
	{
		string temp;

		cout << "\nEnter " << question << " : ";
		getline(cin, temp);

		return temp;
	}
	void processAssignTicket(ServiceTrackerApplication* _tracker)
	{
		if (!_tracker->isloggedInUserAdministrator())
		{
			processMessage(NO_PERMISSION);
			return;
		}

		string ticketid = inputFromUser("ticket id");
		if (!_tracker->isTicketValid(ticketid))
		{
			processMessage(INVALID_TICKET);
			return;
		}

		string userid = inputFromUser("user id");
		if (!_tracker->isValidUser(userid))
		{
			processMessage(INVALID_USER);
			return;
		}

		int ret = _tracker->assignTicket(ticketid, userid);

		if (ret == SUCCESS)
			_tracker->saveticketservices();

		processMessage(ret);
	}
	void processClockIn(ServiceTrackerApplication* _tracker)
	{
		string ticketid = inputFromUser("ticket id");
		if (!_tracker->isTicketValid(ticketid))
		{
			processMessage(INVALID_TICKET);
			return;
		}

		if (!_tracker->isTicketAssigned(ticketid))
		{
			processMessage(TICKET_NOT_ASSIGNED);
			return;
		}

		int servicetype;
		int clockintime;

		cout << "\nEnter service type [1.Installation 2.Repair 3.Milege 4.OnCall 5.Misc] :";
		cin >> servicetype;

		cout << "\nEnter clock in time : ";
		cin >> clockintime;

		int ret = _tracker->clockInService(ticketid, to_string(servicetype), clockintime);

		if (ret == SUCCESS)
			_tracker->saveticketservices();

		processMessage(ret);
	}
	void processClockOut(ServiceTrackerApplication* _tracker)
	{
		string ticketid = inputFromUser("ticket id");
		if (!_tracker->isTicketValid(ticketid))
		{
			processMessage(INVALID_TICKET);
			return;
		}

		if (!_tracker->isTicketAssigned(ticketid))
		{
			processMessage(TICKET_NOT_ASSIGNED);
			return;
		}

		string serviceid = inputFromUser("service id");
		if (!_tracker->isValidServiceForTicket(ticketid, serviceid))
		{
			processMessage(INVALID_SERVICEID);
			return;
		}

		int clockouttime;
		cout << "\nEnter clock out time : ";
		cin >> clockouttime;

		int ret = _tracker->clockOutService(ticketid, serviceid, clockouttime);

		if (ret == SUCCESS)
			_tracker->saveticketservices();

		processMessage(ret);
	}
	void processUpdateNotes(ServiceTrackerApplication* _tracker)
	{
		string ticketid = inputFromUser("ticket id");
		if (!_tracker->isTicketValid(ticketid))
		{
			processMessage(INVALID_TICKET);
			return;
		}

		if (!_tracker->isTicketAssigned(ticketid))
		{
			processMessage(TICKET_NOT_ASSIGNED);
			return;
		}

		string serviceid = inputFromUser("service id");
		if (!_tracker->isValidServiceForTicket(ticketid, serviceid))
		{
			processMessage(INVALID_SERVICEID);
			return;
		}

		string notes = inputFromUser("notes");

		int ret = _tracker->appendNotesToService(ticketid, serviceid, notes);

		if (ret == SUCCESS)
			_tracker->saveticketservices();

		processMessage(ret);
	}
	void processDelete(ServiceTrackerApplication* _tracker)
	{
		string ticketid = inputFromUser("ticket id");
		if (!_tracker->isTicketValid(ticketid))
		{
			processMessage(INVALID_TICKET);
			return;
		}

		if (!_tracker->isTicketAssigned(ticketid))
		{
			processMessage(TICKET_NOT_ASSIGNED);
			return;
		}

		string serviceid = inputFromUser("service id");
		if (!_tracker->isValidServiceForTicket(ticketid, serviceid))
		{
			processMessage(INVALID_SERVICEID);
			return;
		}

		int ret = _tracker->deleteService(ticketid, serviceid);

		if (ret == SUCCESS)
			_tracker->saveticketservices();

		processMessage(ret);
	}
	void processServiceReportBasedOnTicketID(ServiceTrackerApplication* _tracker)
	{
		string ticketid = inputFromUser("ticket id");

		if (!_tracker->isTicketValid(ticketid))
		{
			processMessage(INVALID_TICKET);
			return;
		}

		_tracker->writeServicesPerformedOnTicketIDTo(ticketid, printFileName);
		printtoscreen();
	}
	void processServiceReportBasedOnUserID(ServiceTrackerApplication* _tracker)
	{
		string userid = inputFromUser("user id");

		if (!_tracker->isValidUser(userid))
		{
			processMessage(INVALID_USER);
			return;
		}

		_tracker->writeServicesPerformedOnUserIDTo(userid, printFileName);
		printtoscreen();
	}
};

int main()
{
	ServiceTrackerApplication *_tracker = new ServiceTrackerApplication();
	TestUI *ui = new TestUI(_tracker);

	ui->showUI();

	system("PAUSE");
}
