#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include <chrono>
#include <iostream>

#include "OrderCache.h"

// Demonstrate some basic assertions.
TEST_CASE("AddTwoOrdersShouldMatch", "[ordercache]") {
  OrderCache cache;
  auto order1 = Order{"ID_1", "SECURITYID_1", "Buy", 1000, "User_1", "A"};
  auto order2 = Order{"ID_2", "SECURITYID_1", "Sell", 1000, "User_1", "B"};

  cache.addOrder(std::move(order1));
  cache.addOrder(std::move(order2));
  REQUIRE(1000 == cache.getMatchedQuantity("SECURITYID_1"));
}

TEST_CASE("GivenExampleTest", "[ordercache]") {
  OrderCache cache;
  // OrdId1 US9128473801 Buy  1000 User1 CompanyA
  // OrdId2 US5422358DA3 Sell 3000 User2 CompanyB
  // OrdId3 US9128473801 Sell  500 User3 CompanyA
  // OrdId4 US5422358DA3 Buy   600 User4 CompanyC
  // OrdId5 US5422358DA3 Buy   100 User5 CompanyB
  // OrdId6 US19635GY645 Buy  1000 User6 CompanyD
  // OrdId7 US5422358DA3 Buy  2000 User7 CompanyE
  // OrdId8 US5422358DA3 Sell 5000 User8 CompanyE


  cache.addOrder(Order{"OrdId1", "US9128473801", "Buy",  1000, "User1", "CompanyA"});
  cache.addOrder(Order{"OrdId2", "US5422358DA3", "Sell", 3000, "User2", "CompanyB"});
  cache.addOrder(Order{"OrdId3", "US9128473801", "Sell", 500,  "User3", "CompanyA"});
  cache.addOrder(Order{"OrdId4", "US5422358DA3", "Buy",  600,  "User4", "CompanyC"});
  cache.addOrder(Order{"OrdId5", "US5422358DA3", "Buy",  100,  "User5", "CompanyB"});
  cache.addOrder(Order{"OrdId6", "US19635GY645", "Buy",  1000, "User6", "CompanyD"});
  cache.addOrder(Order{"OrdId7", "US5422358DA3", "Buy",  2000, "User7", "CompanyE"});
  cache.addOrder(Order{"OrdId8", "US5422358DA3", "Sell", 5000, "User8", "CompanyE"});
  REQUIRE(cache.getMatchedQuantity("US5422358DA3") == 2700);
  REQUIRE(cache.getMatchedQuantity("US19635GY645") == 0);
  REQUIRE(cache.getMatchedQuantity("US9128473801") == 0);
}

TEST_CASE("AddTwoOrdersAndRemoveOneShouldNotMatch", "[ordercache]") {
  OrderCache cache;

  cache.addOrder(Order{"ID_1", "SECURITYID_1", "Buy", 1000, "User_1", "A"});
  cache.addOrder(Order{"ID_2", "SECURITYID_1", "Sell", 1000, "User_1", "B"});
  REQUIRE(cache.getMatchedQuantity("SECURITYID_1") == 1000);

  cache.cancelOrder("ID_1");
  REQUIRE(cache.getMatchedQuantity("SECURITYID_1") == 0);
}

TEST_CASE("RemoveUnmatchedOrderShouldNotMatch", "[ordercache]") {
  OrderCache cache;

  cache.addOrder(Order{"ID_1", "SECURITYID_1", "Buy", 1000, "User_1", "A"});
  cache.cancelOrder("ID_1");

  cache.addOrder(Order{"ID_2", "SECURITYID_1", "Sell", 1000, "User_1", "B"});

  REQUIRE(cache.getMatchedQuantity("SECURITYID_1") == 0);
}

TEST_CASE("AddTwoSellAndRemoveOneSellShouldMatchOnlyOne", "[ordercache]") {
  OrderCache cache;

  cache.addOrder(Order{"ID_1", "SECURITYID_1", "Sell", 1000, "User_1", "B"});
  cache.addOrder(Order{"ID_2", "SECURITYID_1", "Buy", 1000, "User_1", "A"});
  cache.addOrder(Order{"ID_3", "SECURITYID_2", "Sell", 1000, "User_1", "B"});
  cache.addOrder(Order{"ID_4", "SECURITYID_2", "Buy", 1000, "User_1", "A"});
  REQUIRE(cache.getMatchedQuantity("SECURITYID_1") == 1000);
  REQUIRE(cache.getMatchedQuantity("SECURITYID_2") == 1000);

  cache.cancelOrder("ID_1");
  REQUIRE(cache.getMatchedQuantity("SECURITYID_1") == 0);
  cache.cancelOrder("ID_3");
  REQUIRE(cache.getMatchedQuantity("SECURITYID_2") == 0);
}

TEST_CASE("BuyOrderDistributedAccross3SellOrder", "[ordercache]") {
  OrderCache cache;

  cache.addOrder(Order{"ID_1", "SECURITYID_1", "Buy", 10000, "User_1", "A"});
  cache.addOrder(Order{"ID_2", "SECURITYID_1", "Sell", 3000, "User_1", "B"});
  cache.addOrder(Order{"ID_3", "SECURITYID_1", "Sell", 3000, "User_4", "B"});
  cache.addOrder(Order{"ID_4", "SECURITYID_1", "Sell", 3000, "User_2", "B"});

  REQUIRE(cache.getMatchedQuantity("SECURITYID_1") == 9000);
}

TEST_CASE("SellOrderDistributedAccross3BuyOrder", "[ordercache]") {
  OrderCache cache;

  cache.addOrder(Order{"ID_1", "SECURITYID_1", "Sell", 10000, "User_1", "A"});
  cache.addOrder(Order{"ID_2", "SECURITYID_1", "Buy", 3000, "User_1", "B"});
  cache.addOrder(Order{"ID_3", "SECURITYID_1", "Buy", 3000, "User_4", "B"});
  cache.addOrder(Order{"ID_4", "SECURITYID_1", "Buy", 3000, "User_2", "B"});

  REQUIRE(cache.getMatchedQuantity("SECURITYID_1") == 9000);
}

TEST_CASE("BuyOrderDistributedAccross3SellOrderCancelOneSellOrder", "[ordercache]") {
  OrderCache cache;

  cache.addOrder(Order{"ID_1", "SECURITYID_1", "Buy", 10000, "User_1", "A"});
  cache.addOrder(Order{"ID_2", "SECURITYID_1", "Sell", 3000, "User_1", "B"});
  cache.addOrder(Order{"ID_3", "SECURITYID_1", "Sell", 3000, "User_4", "B"});
  cache.addOrder(Order{"ID_4", "SECURITYID_1", "Sell", 3000, "User_2", "B"});
  cache.cancelOrder("ID_4");

  REQUIRE(cache.getMatchedQuantity("SECURITYID_1") == 6000);
}

TEST_CASE("BuyOrderDistributedAccross3SellOrderCancelOneSellOrderAndAddAnotherOne", "[ordercache]") {
  OrderCache cache;

  cache.addOrder(Order{"ID_1", "SECURITYID_1", "Buy", 10000, "User_1", "A"});
  cache.addOrder(Order{"ID_2", "SECURITYID_1", "Sell", 3000, "User_1", "B"});
  cache.addOrder(Order{"ID_3", "SECURITYID_1", "Sell", 3000, "User_4", "B"});
  cache.addOrder(Order{"ID_4", "SECURITYID_1", "Sell", 3000, "User_2", "B"});
  cache.cancelOrder("ID_4");

  REQUIRE(cache.getMatchedQuantity("SECURITYID_1") == 6000);

  cache.addOrder(Order{"ID_5", "SECURITYID_1", "Sell", 6000, "User_2", "B"});
  REQUIRE(cache.getMatchedQuantity("SECURITYID_1") == 10000);
}

TEST_CASE("RemoveAtGivenQuantity", "[ordercache]") {
  OrderCache cache;

  cache.addOrder(Order{"ID_1", "SECURITYID_1", "Buy", 10000, "User_1", "A"});
  cache.addOrder(Order{"ID_2", "SECURITYID_1", "Sell", 2000, "User_1", "B"});
  cache.addOrder(Order{"ID_3", "SECURITYID_1", "Sell", 3000, "User_4", "B"});
  cache.addOrder(Order{"ID_4", "SECURITYID_1", "Sell", 3000, "User_2", "B"});

  REQUIRE(cache.getMatchedQuantity("SECURITYID_1") == 8000);
  
  cache.cancelOrdersAtQuantity("SECURITYID_1", 2000);
  REQUIRE(cache.getMatchedQuantity("SECURITYID_1") == 6000);
}

TEST_CASE("RemoveAboveGivenQuantity", "[ordercache]") {
  OrderCache cache;

  cache.addOrder(Order{"ID_1", "SECURITYID_1", "Sell", 2000, "User_1", "A"});
  cache.addOrder(Order{"ID_1", "SECURITYID_1", "Sell", 2000, "User_1", "A"});
  cache.addOrder(Order{"ID_1", "SECURITYID_1", "Sell", 2000, "User_1", "A"});
  cache.addOrder(Order{"ID_1", "SECURITYID_1", "Sell", 2000, "User_1", "A"});
  cache.addOrder(Order{"ID_2", "SECURITYID_1", "Buy", 2000, "User_1", "B"});
  cache.addOrder(Order{"ID_3", "SECURITYID_1", "Buy", 2501, "User_4", "B"});
  cache.addOrder(Order{"ID_4", "SECURITYID_1", "Buy", 3000, "User_2", "B"});

  REQUIRE(cache.getMatchedQuantity("SECURITYID_1") == 7501);
  
  cache.cancelOrdersAboveQuantity("SECURITYID_1", 2500);
  REQUIRE(cache.getMatchedQuantity("SECURITYID_1") == 2000);
}

TEST_CASE("AddOrderPerformanceTest", "[performance]") {
  constexpr int NUM_OF_SAMPLES = 10000;
  OrderCache cache;

  for(int i = 0; i < 10000; ++i){

    cache.addOrder(Order{"SELL" + std::to_string(i), "SECURITYID_1", "Sell", 1000, "User_1", "COMP_" + std::to_string(i)});
  }

  //prepare ids:
  std::vector<std::string> ids;
  for(int i = 0; i < NUM_OF_SAMPLES; ++i){
    ids.push_back(std::to_string(i));
  }


  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
  for(int i = 0; i < NUM_OF_SAMPLES; ++i){
    cache.addOrder(Order{ids[i], "SECURITYID_1", "Buy", 10, "User_1", "B"});
  }
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

  std::cout << NUM_OF_SAMPLES << " orders added in: " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;

}

TEST_CASE("RemoveGivenOrderIDPerformanceTest", "[performance]") {
  constexpr int NUM_OF_SAMPLES = 1000;
  OrderCache cache;

  for(int i = 0; i < 1000; ++i){
    cache.addOrder(Order{"SELL" + std::to_string(i), "SECURITYID_1", "Sell", 1000, "User_1", "COMP_" + std::to_string(i)});
  }

  //prepare ids:
  std::vector<std::string> ids;
  for(int i = 0; i < NUM_OF_SAMPLES; ++i){
    ids.push_back(std::to_string(i));
  }

  for(int i = 0; i < NUM_OF_SAMPLES; ++i){
    cache.addOrder(Order{ids[i], "SECURITYID_1", "Buy", 10, "User_1", "B"});
  }

  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
  for(int i = 0; i < NUM_OF_SAMPLES; ++i){
    cache.cancelOrder(ids[i]);
  }
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

  std::cout << NUM_OF_SAMPLES << " Orders removed in: " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;

}

TEST_CASE("RemoveAboveGivenQuantityPerformanceTest", "[performance]") {
  constexpr int NUM_OF_SAMPLES = 1000;
  OrderCache cache;

  for(int i = 0; i < 1; ++i){
    cache.addOrder(Order{"SELL" + std::to_string(i), "SECURITYID_1", "Sell", 1000, "User_1", "COMP_" + std::to_string(i)});
  }

  //prepare ids:
  std::vector<std::string> ids;
  for(int i = 0; i < NUM_OF_SAMPLES; ++i){
    ids.push_back(std::to_string(i));
  }

  for(int i = 0; i < NUM_OF_SAMPLES; ++i){
    cache.addOrder(Order{ids[i], "SECURITYID_1", "Buy", 100, "User_1", "B"});
  }

  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
  for(int i = 0; i < NUM_OF_SAMPLES; ++i){
    cache.cancelOrdersAboveQuantity("SECURITYID_1", 1);
  }
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

  std::cout << NUM_OF_SAMPLES << " Removals above quantity performed in: " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;

}