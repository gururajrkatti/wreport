#ifndef WREPORT_TABLEDIR_H
#define WREPORT_TABLEDIR_H

#include <wreport/tableinfo.h>
#include <string>
#include <vector>

namespace wreport {
struct Vartable;
struct DTable;

namespace tabledir {
struct Index;

struct Table
{
    std::string btable_id;
    std::string btable_pathname;
    std::string dtable_id;
    std::string dtable_pathname;

    Table(const std::string& dirname, const std::string& filename);
    virtual ~Table() {}

    virtual void print_id(FILE* out) const;
};

/// Information about a version of a BUFR table
struct BufrTable : Table
{
    BufrTableID id;

    BufrTable(const BufrTableID& id, const std::string& dirname, const std::string& filename)
        : Table(dirname, filename), id(id) {}

    void print_id(FILE* out) const override;
};

/// Information about a version of a CREX table
struct CrexTable : Table
{
    CrexTableID id;

    CrexTable(const CrexTableID& id, const std::string& dirname, const std::string& filename)
        : Table(dirname, filename), id(id) {}

    void print_id(FILE* out) const override;
};


/// Indexed version of a table directory
struct Dir
{
    std::string pathname;
    time_t mtime;
    std::vector<Table*> tables;

    Dir(const std::string& pathname);
    Dir(const Dir&) = delete;
    Dir(Dir&&) = default;
    ~Dir();

    Dir& operator=(const Dir&) = delete;

    /// Reread the directory contents if it has changed
    void refresh();
};

struct Query
{
    BufrTable* bufr_best = nullptr;
    CrexTable* crex_best = nullptr;

    void search(Dir& dir);

    Table* result() const;

    virtual bool is_acceptable(const BufrTableID& id) const = 0;
    virtual bool is_acceptable(const CrexTableID& id) const = 0;
    virtual BufrTable* choose_best(BufrTable& first, BufrTable& second) const = 0;
    virtual CrexTable* choose_best(CrexTable& first, CrexTable& second) const = 0;
    virtual Table* choose_best(BufrTable& first, CrexTable& second) const = 0;
};

/// Query for a BUFR table
struct BufrQuery : public Query
{
    BufrTableID id;

    BufrQuery(const BufrTableID& id);

    bool is_acceptable(const BufrTableID& id) const override;
    bool is_acceptable(const CrexTableID& id) const override;
    BufrTable* choose_best(BufrTable& first, BufrTable& second) const override;
    CrexTable* choose_best(CrexTable& first, CrexTable& second) const override;
    Table* choose_best(BufrTable& first, CrexTable& second) const override;
};

/// Query for a CREX table
struct CrexQuery : public Query
{
    CrexTableID id;

    CrexQuery(const CrexTableID& id);

    bool is_acceptable(const BufrTableID& id) const override;
    bool is_acceptable(const CrexTableID& id) const override;
    BufrTable* choose_best(BufrTable& first, BufrTable& second) const override;
    CrexTable* choose_best(CrexTable& first, CrexTable& second) const override;
    Table* choose_best(BufrTable& first, CrexTable& second) const override;
};

class Tabledir
{
protected:
    std::vector<std::string> dirs;
    Index* index;

public:
    Tabledir();
    Tabledir(const Tabledir&) = delete;
    ~Tabledir();

    Tabledir& operator=(const Tabledir&) = delete;

    /**
     * Add the default directories according to compile-time and environment
     * variables.
     */
    void add_default_directories();

    /// Add a table directory to this collection
    void add_directory(const std::string& dir);

    /// Find a BUFR table
    const tabledir::Table* find_bufr(const BufrTableID& id);

    /// Find a CREX table
    const tabledir::Table* find_crex(const CrexTableID& id);

    /// Find a BUFR or CREX table by file name
    const tabledir::Table* find(const std::string& basename);

    /// Print a list of all tables found
    void print(FILE* out);

    /// Get the default tabledir instance
    static Tabledir& get();
};

}
}

#endif
