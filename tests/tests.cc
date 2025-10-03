#ifndef CATCH_CONFIG_MAIN
#  define CATCH_CONFIG_MAIN
#endif

#include "atm.hpp"
#include "catch.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////
//                             Helper Definitions //
/////////////////////////////////////////////////////////////////////////////////////////////

bool CompareFiles(const std::string& p1, const std::string& p2) {
  std::ifstream f1(p1);
  std::ifstream f2(p2);

  if (f1.fail() || f2.fail()) {
    return false;  // file problem
  }

  std::string f1_read;
  std::string f2_read;
  while (f1.good() || f2.good()) {
    f1 >> f1_read;
    f2 >> f2_read;
    if (f1_read != f2_read || (f1.good() && !f2.good()) ||
        (!f1.good() && f2.good()))
      return false;
  }
  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Test Cases
/////////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Example: Create a new account", "[ex-1]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  auto accounts = atm.GetAccounts();
  REQUIRE(accounts.contains({12345678, 1234}));
  REQUIRE(accounts.size() == 1);

  Account sam_account = accounts[{12345678, 1234}];
  REQUIRE(sam_account.owner_name == "Sam Sepiol");
  REQUIRE(sam_account.balance == 300.30);

  auto transactions = atm.GetTransactions();
  REQUIRE(accounts.contains({12345678, 1234}));
  REQUIRE(accounts.size() == 1);
  std::vector<std::string> empty;
  REQUIRE(transactions[{12345678, 1234}] == empty);
}

TEST_CASE("Example: Simple widthdraw", "[ex-2]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  atm.WithdrawCash(12345678, 1234, 20);
  auto accounts = atm.GetAccounts();
  Account sam_account = accounts[{12345678, 1234}];

  REQUIRE(sam_account.balance == 280.30);
}

TEST_CASE("Example: Print Prompt Ledger", "[ex-3]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  auto& transactions = atm.GetTransactions();
  transactions[{12345678, 1234}].push_back(
      "Withdrawal - Amount: $200.40, Updated Balance: $99.90");
  transactions[{12345678, 1234}].push_back(
      "Deposit - Amount: $40000.00, Updated Balance: $40099.90");
  transactions[{12345678, 1234}].push_back(
      "Deposit - Amount: $32000.00, Updated Balance: $72099.90");
  atm.PrintLedger("./prompt.txt", 12345678, 1234);
  REQUIRE(CompareFiles("./ex-1.txt", "./prompt.txt"));
}
TEST_CASE("Check Accounts Map", "[map1]") {
  Atm atm;
  atm.RegisterAccount(11112222, 9876, "Hacker Smith", 3000);
  auto& accts = atm.GetAccounts();
  REQUIRE(accts.find({11112222, 9876}) != accts.end());
}
TEST_CASE("Duplicate Accounts", "[dup1]") {
  Atm atm;
  atm.RegisterAccount(17171981, 9087, "Billy Joe", 8900);
  size_t b4 = atm.GetAccounts().size();
  try {
    atm.RegisterAccount(17171981, 9087, "Billy Joe", 8900);
  } catch (std::invalid_argument& e) {
    // Nothing
  }
  REQUIRE(b4 == atm.GetAccounts().size());
}
TEST_CASE("Infinite Money", "[inf1]") {
  Atm atm;
  atm.RegisterAccount(6785689456, 4563, "Idk atp", 7890);
  atm.WithdrawCash(6785689456, 4563, 500);
  REQUIRE(atm.CheckBalance(6785689456, 4563) == 7390);
}
TEST_CASE("Negative Balance Check", "[neg1]") {
  Atm atm;
  atm.RegisterAccount(5678345, 8734, "Lmfao what", 1000);
  try {
    atm.WithdrawCash(5678345, 8734, 1500);
  } catch (std::runtime_error& e) {
    // Nothing
  }
  REQUIRE(atm.CheckBalance(5678345, 8734) == 1000);
}
TEST_CASE("Negative Removal", "[neg2]") {
  Atm atm;
  atm.RegisterAccount(87654432, 8967, "Billy Joe", 8900);
  try {
    atm.WithdrawCash(87654432, 8967, -1500);
  } catch (std::invalid_argument& e) {
    // Nothing
  }
  REQUIRE(atm.CheckBalance(87654432, 8967) == 8900);
}
TEST_CASE("Register Negative Balance", "[regneg1]") {
  Atm atm;
  atm.RegisterAccount(9479370632, 8345, "Negative nancy", -1900);
  auto& accts = atm.GetAccounts();
  REQUIRE(accts.find({9479370632, 8345}) != accts.end());
  REQUIRE(accts[{9479370632, 8345}].balance == -1900);
  REQUIRE(accts[{9479370632, 8345}].owner_name == "Negative nancy");
}
TEST_CASE("Deposit updated balance", "[dep1]") {
  Atm atm;
  atm.RegisterAccount(234567986, 3456, "Running Man", 9000);
  atm.DepositCash(234567986, 3456, 6000);
  REQUIRE(atm.CheckBalance(234567986, 3456) == 15000);
}
TEST_CASE("Negative Deposit", "[negativedep1]") {
  Atm atm;
  atm.RegisterAccount(70707070, 1212, "Gracey", 400);
  try {
    atm.DepositCash(70707070, 1212, -10.0);
  } catch (std::exception& e) {
    // Nothing!!
  }
  REQUIRE(atm.CheckBalance(70707070, 1212) == 400);
}
TEST_CASE("Ledger", "[led]") {
  Atm atm;
  atm.RegisterAccount(20221010, 1010, "Henry", 200.0);
  atm.DepositCash(20221010, 1010, 100.0);
  atm.WithdrawCash(20221010, 1010, 50.0);
  atm.PrintLedger("ledger_test.txt", 20221010, 1010);
}
