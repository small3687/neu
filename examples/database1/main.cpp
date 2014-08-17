#include <iostream>

#include <neu/nvar.h>
#include <neu/NProgram.h>
#include <neu/NDatabase.h>
#include <neu/NRandom.h>
#include <neu/NSys.h>

using namespace std;
using namespace neu;

static const size_t NUM_ROWS = 2000;
static const size_t COMMIT_INTERVAL = 100;

int main(int argc, char** argv){
  NProgram program(argc, argv);

  // does the database already exist?
  bool create = !NSys::exists("testdb");

  // if not, create it in the current directory
  NDatabase db("testdb", create);

  // limit pageable memory use to 1024 MB
  // larger sizes make reads and writes faster.
  // the memory limit can be changed at any time
  db.setMemoryLimit(1024);

  // test with lower values to see how reads/writes perform in the
  // presence of more page swapping
  //db.setMemoryLimit(128);

  if(create){
    // create a new table named "users"
    NTable* table = db.addTable("users");

    // get a write lock on the table using NTable::Lock. you can
    // acquire multiple read and write locks in an order consistent
    // with how other locks are obtained by others clients of the
    // database to help prevent deadlocking, i.e: table locks will
    // always be acquired in the same relative order. the lock
    // autoreleaes its locks on the underlying tables when the
    // NTable::Lock is destroyed
    NTable::Lock lock(table, true);

    // add indexable fields for fast queries
    table->addIndex("rank", NTable::Double);

    // the hash index is the catch-all, it can hash any nvar
    // but a hash index will not be sorted by its nvar value in
    // a meaningful way that supports ranged based queries
    table->addIndex("name", NTable::Hash);

    NRandom rng;

    double t1 = NSys::now();
    
    nvar row;

    // add some rows to the table
    for(size_t i = 1; i < NUM_ROWS; ++i){
      row("rank") = rng.uniform(0, 100);
      row("name") = "neu" + nvar(i);

      // extra unindexed payload data - can be any type of potentially
      // deep and nested nvar
      row("norm") = rng.uniform(0, 100);

      if(i % 1000 == 0){
        nvar p;
        p("f1") = "t1";
        p("v") = nvec() << (i + 1) << 2 << 3;
        row("payload1") = p;
      }

      // note, any additional payload fields can be added on a per-row
      // basis

      table->insert(row);
      
      if(i % COMMIT_INTERVAL == COMMIT_INTERVAL - 1){
        table->commit();
        cout << "commiting..." << endl;
      }
    }

    table->dump();

    nvar u3;
    // get the user whose name is neu123
    table->get("name", "neu123", u3);  
    // update the user's rank

    cout << "u3 is: " << u3 << endl;

    return 0;

    double dt = NSys::now() - t1;

    cout << "inserted " << NUM_ROWS << " rows in: " << dt << endl;

    // add some more rows
    //for(size_t i = 1; i < 10000; ++i){
    //  row("rank") = rng.uniform(0, 100);
    //  row("norm") = rng.uniform(0, 100);
    // }

    // this time we will not commit the rows just added, instead
    // rollback
    //    table->rollback();

    /*
    nvar u2;
    // get the user whose name is neu111
    table->get("name", "neu111", u2);  
    // erase this user
    table->erase(u2["id"]);
    */

    //nvar u3;
    // get the user whose name is neu123
    //table->get("name", "neu123", u3);  
    // update the user's rank

    //cout << "u3 is: " << u3 << endl;

    //u3["rank"] = 87.0;
    //table->update(u3);

    // make sure everything is committed - when the NDatbase instance
    // is destroyed it will autocommit anyways
    db.commit();

    cout << "run command again to query" << endl;
    
    return 0;
  }

  // we opened the database, now perform some queries on it
  NTable* table = db.getTable("users");

  // get a read lock on the table
  NTable::Lock lock(table);
  
  nvec rows;

  double t1 = NSys::now();

  // a query func is a lambda which receives a const nvar& r for the
  // row - it returns 0 to stop querying, 1 to move forward, -1 to
  // return backwards
  NTable::QueryFunc q1 =
    [&](const nvar& r){
    if(r["norm"] < 50){
      rows << r;
    }
    return 1;
  };

  // find all rows whose rank >= 99.9 and norm < 50
  table->query("rank", 99.9, q1);

  nvar u1;
  // get the user whose name is neu9999
  table->get("name", "neu9999", u1);

  // do "index" queries - we will fetch row id's not the actual row

  NTable::RowSet r1;
  // row ids of users whose: 0 <= rank <= 0.01
  table->setQuery("rank", 0, 0.01, r1);

  NTable::RowSet r2;
  // row ids of users whose: 50 <= rank <= 50.01
  table->setQuery("rank", 50, 50.01, r2);
  
  // we can do set operations: unite() for union, intersect() for
  // interesect, complement() for set difference
  r1.unite(r2);

  nvec rows2;
  NTable::QueryFunc q2 =
    [&](const nvar& r){
    rows2 << r;
    return 1;
  };

  // fetch the actual rows
  table->get(r1, q2);

  size_t count = 0;
  NTable::QueryFunc q3 =
    [&](const nvar& r){

    if(count > 100){
      return 0;
    }

    //cout << "r is: " << r << endl;

    ++count;
    return 1;
  };

  // traverse the first 100 rows - they could be in any order
  table->traverseStart(q3);

  //cout << "rows2 is: " << rows2 << endl;

  double dt = NSys::now() - t1;

  //cout << "rows is: " << rows << endl;

  //cout << "u1 is: " << u1 << endl;

  cout << "queries took: " << dt << endl;

  return 0;
}
