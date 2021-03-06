#include "tests.h"
#include "tabledir.h"
#include "vartable.h"
#include <set>

using namespace wreport;
using namespace wreport::tests;
using namespace std;

namespace {

class Tests : public TestCase
{
    using TestCase::TestCase;

    void register_tests() override
    {
        add_method("internals", []() {
            // Test Table, BufrTable, CrexTable
            using namespace wreport::tabledir;

            BufrTable t(BufrTableID(0, 0, 0, 0, 0), "/antani", "B12345.txt");
            wassert(actual(t.btable_id) == "B12345");
            wassert(actual(t.dtable_id) == "D12345");
            wassert(actual(t.btable_pathname) == "/antani/B12345.txt");
            wassert(actual(t.dtable_pathname) == "/antani/D12345.txt");
        });
        add_method("tabledir_bufr_wmo", []() {
            auto& td = tabledir::Tabledirs::get();

            const tabledir::Table* t = td.find_bufr(BufrTableID(0, 0, 0, 10, 0));
            wassert(actual(t != 0).istrue());
            const tabledir::BufrTable* bt = dynamic_cast<const tabledir::BufrTable*>(t);
            wassert(actual(bt != 0).istrue());
            wassert(actual(bt->id.originating_centre) == 0);
            wassert(actual(bt->id.originating_subcentre) == 0);
            wassert(actual(bt->id.master_table_number) == 0);
            wassert(actual(bt->id.master_table_version_number) == 11);
            wassert(actual(bt->id.master_table_version_number_local) == 0);
        });
        add_method("tabledir_bufr_ecmwf", []() {
            auto& td = tabledir::Tabledirs::get();
            const tabledir::Table* t = td.find_bufr(BufrTableID(98, 0, 0, 6, 1));
            wassert(actual(t != 0).istrue());
            const tabledir::BufrTable* bt = dynamic_cast<const tabledir::BufrTable*>(t);
            wassert(actual(bt != 0).istrue());
            wassert(actual((int)bt->id.originating_centre) == 98);
            wassert(actual((int)bt->id.originating_subcentre) == 0);
            wassert(actual((int)bt->id.master_table_number) == 0);
            wassert(actual((int)bt->id.master_table_version_number) == 6);
            wassert(actual((int)bt->id.master_table_version_number_local) == 1);
        });
        add_method("tabledir_crex_old", []() {
            auto& td = tabledir::Tabledirs::get();
            const tabledir::Table* t = td.find_crex(CrexTableID(1, 0, 0, 0, 3, 0, 0));
            wassert(actual(t != 0).istrue());
            const tabledir::CrexTable* ct = dynamic_cast<const tabledir::CrexTable*>(t);
            wassert(actual(ct != 0).istrue());
            wassert(actual((int)ct->id.originating_centre) == 0);
            wassert(actual((int)ct->id.originating_subcentre) == 0);
            wassert(actual((int)ct->id.master_table_number) == 0);
            wassert(actual((int)ct->id.master_table_version_number) == 3);
            wassert(actual((int)ct->id.master_table_version_number_local) == 0);
            wassert(actual((int)ct->id.master_table_version_number_bufr) == 0);
        });
        add_method("tabledir_crex_bufr", []() {
            // Load the same table file as bufr and as crex, and check the
            // differences
            auto& td = tabledir::Tabledirs::get();

            //td.explain_find_bufr(BufrTableID(0, 0, 0, 24, 0), stderr);

            const tabledir::Table* t = td.find_bufr(BufrTableID(0, 0, 0, 24, 0));
            wassert(actual(t != 0).istrue());
            const tabledir::BufrTable* bt = dynamic_cast<const tabledir::BufrTable*>(t);
            wassert(actual(bt != 0).istrue());
            wassert(actual((int)bt->id.originating_centre) == 0);
            wassert(actual((int)bt->id.originating_subcentre) == 0);
            wassert(actual((int)bt->id.master_table_number) == 0);
            wassert(actual((int)bt->id.master_table_version_number) == 24);
            wassert(actual((int)bt->id.master_table_version_number_local) == 0);
            const Vartable* vt = Vartable::load_bufr(t->btable_pathname);
            wassert(actual(vt->query(WR_VAR(0, 12, 101))->unit) == "K");

            t = td.find_crex(CrexTableID(1, 0, 0, 0, 24, 24, 0));
            wassert(actual(t != 0).istrue());
            bt = dynamic_cast<const tabledir::BufrTable*>(t);
            wassert(actual(bt != 0).istrue());
            wassert(actual((int)bt->id.originating_centre) == 0);
            wassert(actual((int)bt->id.originating_subcentre) == 0);
            wassert(actual((int)bt->id.master_table_number) == 0);
            wassert(actual((int)bt->id.master_table_version_number) == 24);
            wassert(actual((int)bt->id.master_table_version_number_local) == 0);
            vt = Vartable::load_crex(t->btable_pathname);
            wassert(actual(vt->query(WR_VAR(0, 12, 101))->unit) == "C");
        });
        add_method("tabledir_extra", []() {
            // Find a non-BUFR non-CREX table by name
            auto& td = tabledir::Tabledirs::get();
            const tabledir::Table* t = td.find("test");
            wassert(actual(t != 0).istrue());
        });
    }
} test("tabledir");

}
