#include "databaseaccess.h"

std::string DatabaseAccess::_databasePath = "";

DatabaseAccess &DatabaseAccess::getInstance()
{
	static DatabaseAccess instance;
	return instance;
}

Storage &DatabaseAccess::getStorage()
{
	return DatabaseAccess::getInstance()._storage;
}

void DatabaseAccess::setDatabasePath(std::string databasePath)
{
	_databasePath = databasePath;
}

DatabaseAccess::DatabaseAccess()
	: _storage(initStorage(_databasePath))
{
}

DatabaseAccess::~DatabaseAccess()
{
}

void initDatabaseInner(std::string databasePath)
{
	DatabaseAccess::setDatabasePath(databasePath);
	DatabaseAccess::getInstance();
}
