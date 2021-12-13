#ifndef PAHDB_H_
#define PAHDB_H_

#include "Exception.h"
#include "PAHGeometry.h"

#include <mysql++/mysql++.h>
#include <mysql++/ssqls.h>

sql_create_16(sql_properties, 1, 0, int, uid, int, hydrogen, int, carbon, int,
              nitrogen, int, oxygen, int, magnesium, int, silicate, int, iron,
              int, charge, int, solo, int, duo, int, trio, int, quartet, int,
              quintet, int, ch2, int, chx);

#include <unistd.h>

#include <string>
#include <string_view>
#include <utility>
#include <vector>

class PAHdb {

public:
  enum class Database : char { Theory, Experiment };

  void connect(std::string_view database, const std::string_view host,
               std::string_view username, const std::string_view password,
               int port, bool compress, int timeout,
               const std::string_view socket);

  void setTable(Database table);

  void setProgress(double progress);

  void setError(const char *error);

  std::vector<std::vector<std::pair<double, double>>>
  getTransitionsFromUIDsAndVersion(const std::vector<int> &uids, int version);

  std::vector<std::vector<std::pair<double, double>>>
  getTransitionsFromUIDAndVersion(int uid, int version);

  std::vector<std::vector<std::pair<double, double>>>
  getTransitionsFromVersion(int version, std::vector<int> &ids);

  std::vector<std::vector<std::pair<double, double>>>
  getTransitionsFromIds(const std::vector<int> &ids);

  std::vector<std::vector<std::pair<double, double>>>
  getTransitionsFromId(int id);

  std::vector<std::vector<std::pair<double, double>>>
  getExperimentalAndTheoreticalTransitionsFromId(int id);

  std::vector<PAHGeometry> getGeometriesFromIds(const std::vector<int> &ids);

  std::vector<sql_properties>
  getPropertiesByUIDsAndVersion(const std::vector<int> &uids, int version);

  std::vector<sql_properties> getPropertiesByIDs(const std::vector<int> &ids);

  std::vector<std::string> getFormulaeFromIds(const std::vector<int> &ids);

private:
  mysqlpp::Connection _connection;

  Database _table;
};

inline void PAHdb::setTable(Database table) { _table = table; }

inline std::vector<std::vector<std::pair<double, double>>>
PAHdb::getTransitionsFromId(int id) {
  return (getTransitionsFromIds(std::vector<int>(1, id)));
}

inline std::vector<std::vector<std::pair<double, double>>>
PAHdb::getTransitionsFromUIDAndVersion(int uid, int version) {
  return (getTransitionsFromUIDsAndVersion(std::vector<int>(1, uid), version));
}

#endif /* PAHDB_H_ */
