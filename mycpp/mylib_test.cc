#include <assert.h>
#include <stdarg.h>  // va_list, etc.
#include <stdio.h>   // vprintf

#include "greatest.h"
#include "mylib.h"

// Test helper.  TODO: Could use ASSERT_EQ_T to customize equality and print
// difference.
//
// Example:
//
// https://github.com/silentbicycle/greatest/blob/master/example.c#L71

bool equals_c_string(const char* c_string, Str* s) {
  return str_equals(new Str(c_string), s);
}

TEST test_str_to_int() {
  int i;
  bool ok;

  ok = _str_to_int(new Str("345"), &i, 10);
  ASSERT(ok);
  ASSERT_EQ_FMT(345, i, "%d");

  ok = _str_to_int(new Str("1234567890"), &i, 10);
  ASSERT(ok);
  ASSERT(i == 1234567890);

  // overflow
  ok = _str_to_int(new Str("12345678901234567890"), &i, 10);
  ASSERT(!ok);

  // underflow
  ok = _str_to_int(new Str("-12345678901234567890"), &i, 10);
  ASSERT(!ok);

  // negative
  ok = _str_to_int(new Str("-123"), &i, 10);
  ASSERT(ok);
  ASSERT(i == -123);

  // Leading space is OK!
  ok = _str_to_int(new Str(" -123"), &i, 10);
  ASSERT(ok);
  ASSERT(i == -123);

  // Trailing space is OK!  NOTE: This fails!
  ok = _str_to_int(new Str(" -123  "), &i, 10);
  ASSERT(ok);
  ASSERT(i == -123);

  // Empty string isn't an integer
  ok = _str_to_int(new Str(""), &i, 10);
  ASSERT(!ok);

  ok = _str_to_int(new Str("xx"), &i, 10);
  ASSERT(!ok);

  // Trailing garbage
  ok = _str_to_int(new Str("42a"), &i, 10);
  ASSERT(!ok);

  i = to_int(new Str("ff"), 16);
  ASSERT(i == 255);

  // strtol allows 0x prefix
  i = to_int(new Str("0xff"), 16);
  ASSERT(i == 255);

  // TODO: test ValueError here
  // i = to_int(new Str("0xz"), 16);

  i = to_int(new Str("0"), 16);
  ASSERT(i == 0);

  i = to_int(new Str("077"), 8);
  ASSERT_EQ_FMT(63, i, "%d");

  bool caught = false;
  try {
    i = to_int(new Str("zzz"));
  } catch (ValueError* e) {
    caught = true;
  }
  ASSERT(caught);

  PASS();
}

TEST test_str_funcs() {
  ASSERT(!(new Str(""))->isupper());
  ASSERT(!(new Str("a"))->isupper());
  ASSERT((new Str("A"))->isupper());
  ASSERT((new Str("AB"))->isupper());

  ASSERT((new Str("abc"))->isalpha());

  Str* s = new Str("abc");
  Str* r0 = str_repeat(s, 0);
  ASSERT(equals_c_string("", r0));

  Str* r1 = str_repeat(s, 1);
  ASSERT(equals_c_string("abc", r1));

  Str* r3 = str_repeat(s, 3);
  ASSERT(equals_c_string("abcabcabc", r3));

  Str* int_str;
  int_str = str((1 << 31) - 1);
  ASSERT(str_equals(new Str("2147483647"), int_str));

  // wraps with - sign.
  int_str = str(1 << 31);
  log("i = %s", int_str->data_);

  int_str = str(-(1 << 31) + 1);
  log("i = %s", int_str->data_);
  int_str = str(-(1 << 31));
  log("i = %s", int_str->data_);

  Str* s2 = new Str(" abc ");
  ASSERT(str_equals(new Str(" abc"), s2->rstrip()));

  Str* s3 = new Str(" def");
  ASSERT(str_equals(new Str(" def"), s3->rstrip()));

  Str* s4 = new Str("");
  ASSERT(str_equals(new Str(""), s4->rstrip()));

  Str* s5 = new Str("");
  ASSERT(str_equals(new Str(""), s5->strip()));

  Str* st1 = (new Str(" 123 "))->strip();
  ASSERT(str_equals(new Str("123"), st1));
  Str* st2 = (new Str(" 123"))->strip();
  ASSERT(str_equals(new Str("123"), st2));
  Str* st3 = (new Str("123 "))->strip();
  ASSERT(str_equals(new Str("123"), st3));

  ASSERT(s->startswith(new Str("")));
  ASSERT(s->startswith(new Str("ab")));
  ASSERT(!s->startswith(new Str("bc")));

  ASSERT(s->endswith(new Str("")));
  ASSERT(!s->endswith(new Str("ab")));
  ASSERT(s->endswith(new Str("bc")));

  log("repr %s", repr(new Str(""))->data_);
  log("repr %s", repr(new Str("'"))->data_);
  log("repr %s", repr(new Str("'single'"))->data_);
  log("repr %s", repr(new Str("\"double\""))->data_);

  // this one is truncated
  const char* n_str = "NUL \x00 NUL";
  int n_len = 9;  // 9 bytes long
  log("repr %s", repr(new Str(n_str, n_len))->data_);
  log("len %d", len(repr(new Str(n_str, n_len))));

  log("repr %s", repr(new Str("tab\tline\nline\r\n"))->data_);
  log("repr %s", repr(new Str("high \xFF \xFE high"))->data_);

  ASSERT_EQ(65, ord(new Str("A")));

  log("split_once()");
  Tuple2<Str*, Str*> t = mylib::split_once(new Str("foo=bar"), new Str("="));
  ASSERT(str_equals(t.at0(), new Str("foo")));
  ASSERT(str_equals(t.at1(), new Str("bar")));

  Tuple2<Str*, Str*> u = mylib::split_once(new Str("foo="), new Str("="));
  ASSERT(str_equals(u.at0(), new Str("foo")));
  ASSERT(str_equals(u.at1(), new Str("")));

  Tuple2<Str*, Str*> v = mylib::split_once(new Str("foo="), new Str("Z"));
  ASSERT(str_equals(v.at0(), new Str("foo=")));
  ASSERT(str_equals(v.at1(), new Str("")));

  Tuple2<Str*, Str*> w = mylib::split_once(new Str(""), new Str("Z"));
  ASSERT(str_equals(w.at0(), new Str("")));
  ASSERT(str_equals(w.at1(), new Str("")));

  PASS();
}

using mylib::BufLineReader;

TEST test_buf_line_reader() {
  Str* s = new Str("foo\nbar\nleftover");
  BufLineReader* reader = new BufLineReader(s);
  Str* line;

  log("BufLineReader");

  line = reader->readline();
  log("1 [%s]", line->data_);
  line = reader->readline();
  log("2: [%s]", line->data_);
  line = reader->readline();
  log("3: [%s]", line->data_);
  line = reader->readline();
  log("4: [%s]", line->data_);

  PASS();
}

TEST test_formatter() {
  gBuf.reset();
  gBuf.write_const("[", 1);
  gBuf.format_s(new Str("bar"));
  gBuf.write_const("]", 1);
  log("value = %s", gBuf.getvalue()->data_);

  gBuf.format_d(42);
  gBuf.write_const("-", 1);
  gBuf.format_d(42);
  gBuf.write_const(".", 1);
  log("value = %s", gBuf.getvalue()->data_);

  PASS();
}

TEST test_list_funcs() {
  std::vector<int> v;
  v.push_back(0);
  log("v.size = %d", v.size());
  v.erase(v.begin());
  log("v.size = %d", v.size());

  log("  ints");
  auto ints = new List<int>({1, 2, 3});
  log("-- before pop(0)");
  for (int i = 0; i < len(ints); ++i) {
    log("ints[%d] = %d", i, ints->index(i));
  }
  ints->pop(0);

  log("-- after pop(0)");
  for (int i = 0; i < len(ints); ++i) {
    log("ints[%d] = %d", i, ints->index(i));
  }

  ints->set(0, 42);
  ints->set(1, 43);
  log("-- after mutation");
  for (int i = 0; i < len(ints); ++i) {
    log("ints[%d] = %d", i, ints->index(i));
  }

  auto L = list_repeat<Str*>(nullptr, 3);
  log("list_repeat length = %d", len(L));

  auto L2 = list_repeat<bool>(true, 3);
  log("list_repeat length = %d", len(L2));
  log("item 0 %d", L2->index(0));
  log("item 1 %d", L2->index(1));

  PASS();
}

TEST test_list_iters() {
  log("  forward iteration over list");
  auto ints = new List<int>({1, 2, 3});
  for (ListIter<int> it(ints); !it.Done(); it.Next()) {
    int x = it.Value();
    log("x = %d", x);
  }

  log("  backward iteration over list");
  for (ReverseListIter<int> it(ints); !it.Done(); it.Next()) {
    int x = it.Value();
    log("x = %d", x);
  }

  PASS();
}

TEST test_contains() {
  bool b;

  b = str_contains(new Str("foo"), new Str("oo"));
  ASSERT(b == true);

  b = str_contains(new Str("foo"), new Str("ood"));
  ASSERT(b == false);

  log("  strs");
  auto strs = new List<Str*>();
  strs->append(new Str("bar"));

  b = list_contains(strs, new Str("foo"));
  ASSERT(b == false);

  strs->append(new Str("foo"));
  b = list_contains(strs, new Str("foo"));
  ASSERT(b == true);

  log("  ints");
  auto ints = new List<int>({1, 2, 3});
  b = list_contains(ints, 1);
  ASSERT(b == true);

  b = list_contains(ints, 42);
  ASSERT(b == false);

  log("  floats");
  auto floats = new List<double>({0.5, 0.25, 0.0});
  b = list_contains(floats, 0.0);
  log("b = %d", b);
  b = list_contains(floats, 42.0);
  log("b = %d", b);

  PASS();
}

TEST test_files() {
  mylib::Writer* w = mylib::Stdout();
  bool b = w->isatty();
  log("b = %d", b);

  FILE* f = fopen("README.md", "r");
  auto r = new mylib::CFileLineReader(f);
  // auto r = mylib::Stdin();
  Str* s = r->readline();
  log("test_files");
  println_stderr(s);
  log("test_files DONE");

  PASS();
}

TEST test_dict() {
  // TODO: How to initialize constants?

  // Dict d {{"key", 1}, {"val", 2}};
  Dict<int, Str*>* d = new Dict<int, Str*>();
  d->set(1, new Str("foo"));
  log("d[1] = %s", d->index(1)->data_);

  auto d2 = new Dict<Str*, int>();
  Str* key = new Str("key");
  d2->set(key, 42);
  log("d2['key'] = %d", d2->index(key));
  d2->set(new Str("key2"), 2);
  d2->set(new Str("key3"), 3);

  log("  iterating over Dict");
  for (DictIter<Str*, int> it(d2); !it.Done(); it.Next()) {
    log("k = %s, v = %d", it.Key()->data_, it.Value());
  }

  Str* v1 = d->get(1);
  log("v1 = %s", v1->data_);
  ASSERT(dict_contains(d, 1));
  ASSERT(!dict_contains(d, 2));

  Str* v2 = d->get(423);  // nonexistent
  log("v2 = %p", v2);

  auto d3 = new Dict<Str*, int>();
  d3->set(new Str("a"), 10);
  d3->set(new Str("b"), 11);
  d3->set(new Str("c"), 12);
  log("a = %d", d3->index(new Str("a")));
  log("b = %d", d3->index(new Str("b")));
  log("c = %d", d3->index(new Str("c")));

  PASS();
}

TEST misc_test() {
  List<int>* L = new List<int>{1, 2, 3};

  log("size: %d", len(L));
  log("");

  Tuple2<int, int>* t2 = new Tuple2<int, int>(5, 6);
  log("t2[0] = %d", t2->at0());
  log("t2[1] = %d", t2->at1());

  Tuple2<int, Str*>* u2 = new Tuple2<int, Str*>(42, new Str("hello"));
  log("u2[0] = %d", u2->at0());
  log("u2[1] = %s", u2->at1()->data_);

  log("");

  auto t3 = new Tuple3<int, Str*, Str*>(42, new Str("hello"), new Str("bye"));
  log("t3[0] = %d", t3->at0());
  log("t3[1] = %s", t3->at1()->data_);
  log("t3[2] = %s", t3->at2()->data_);

  log("");

  auto t4 =
      new Tuple4<int, Str*, Str*, int>(42, new Str("4"), new Str("four"), -42);

  log("t4[0] = %d", t4->at0());
  log("t4[1] = %s", t4->at1()->data_);
  log("t4[2] = %s", t4->at2()->data_);
  log("t4[3] = %d", t4->at3());

  // Str = 16 and List = 24.
  // Rejected ideas about slicing:
  //
  // - Use data[len] == '\0' as OWNING and data[len] != '\0' as a slice?
  //   It doesn't work because s[1:] would always have that problem
  //
  // - s->data == (void*)(s + 1)
  //   Owning string has the data RIGHT AFTER?
  //   Maybe works? but probably a bad idea because of GLOBAL Str instances.

  log("");
  log("sizeof(Str) = %zu", sizeof(Str));
  log("sizeof(List<int>) = %zu", sizeof(List<int>));

  PASS();
}

GREATEST_MAIN_DEFS();

int main(int argc, char** argv) {
  GREATEST_MAIN_BEGIN();

  RUN_TEST(misc_test);
  RUN_TEST(test_str_to_int);
  RUN_TEST(test_str_funcs);

  RUN_TEST(test_list_funcs);
  RUN_TEST(test_list_iters);
  RUN_TEST(test_dict);

  RUN_TEST(test_buf_line_reader);
  RUN_TEST(test_formatter);
  RUN_TEST(test_contains);
  RUN_TEST(test_files);

  GREATEST_MAIN_END(); /* display results */
  return 0;
}
