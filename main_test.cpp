#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include <chrono>

#include "OrderCache.h"

// Demonstrate some basic assertions.
TEST_CASE("AddTwoOrdersShouldMatch", "SecurityIdTest") {
  OrderCache cache;
  const auto order1 = Order{"ID_1", "SECURITYID_1", "Buy", 1000, "User_1", "A"};
  const auto order2 = Order{"ID_2", "SECURITYID_1", "Sell", 1000, "User_1", "B"};

  cache.addOrder(order1);
  cache.addOrder(order2);
  REQUIRE(1000 == cache.getMatchedQuantity("SECURITYID_1"));
}

TEST_CASE("GivenExampleTest", "SecurityIdTest") {
  OrderCache cache;
  // OrdId1 US9128473801 Buy  1000 User1 CompanyA
  // OrdId2 US5422358DA3 Sell 3000 User2 CompanyB
  // OrdId3 US9128473801 Sell  500 User3 CompanyA
  // OrdId4 US5422358DA3 Buy   600 User4 CompanyC
  // OrdId5 US5422358DA3 Buy   100 User5 CompanyB
  // OrdId6 US19635GY645 Buy  1000 User6 CompanyD
  // OrdId7 US5422358DA3 Buy  2000 User7 CompanyE
  // OrdId8 US5422358DA3 Sell 5000 User8 CompanyE

  const auto order1 = Order{"OrdId1", "US9128473801", "Buy",  1000, "User1", "CompanyA"};
  const auto order2 = Order{"OrdId2", "US5422358DA3", "Sell", 3000, "User2", "CompanyB"};
  const auto order3 = Order{"OrdId3", "US9128473801", "Sell", 500,  "User3", "CompanyA"};
  const auto order4 = Order{"OrdId4", "US5422358DA3", "Buy",  600,  "User4", "CompanyC"};
  const auto order5 = Order{"OrdId5", "US5422358DA3", "Buy",  100,  "User5", "CompanyB"};
  const auto order6 = Order{"OrdId6", "US19635GY645", "Buy",  1000, "User6", "CompanyD"};
  const auto order7 = Order{"OrdId7", "US5422358DA3", "Buy",  2000, "User7", "CompanyE"};
  const auto order8 = Order{"OrdId8", "US5422358DA3", "Sell", 5000, "User8", "CompanyE"};


  cache.addOrder(order1);
  cache.addOrder(order2);
  cache.addOrder(order3);
  cache.addOrder(order4);
  cache.addOrder(order5);
  cache.addOrder(order6);
  cache.addOrder(order7);
  cache.addOrder(order8);
  REQUIRE(cache.getMatchedQuantity("US5422358DA3") == 2700);
  REQUIRE(cache.getMatchedQuantity("US19635GY645") == 0);
  REQUIRE(cache.getMatchedQuantity("US9128473801") == 0);
}

TEST_CASE("AddTwoOrdersAndRemoveOneShouldNotMatch", "SecurityIdTest") {
  OrderCache cache;
  const auto order1 = Order{"ID_1", "SECURITYID_1", "Buy", 1000, "User_1", "A"};
  const auto order2 = Order{"ID_2", "SECURITYID_1", "Sell", 1000, "User_1", "B"};

  cache.addOrder(order1);
  cache.addOrder(order2);
  REQUIRE(cache.getMatchedQuantity("SECURITYID_1") == 1000);

  cache.cancelOrder("ID_1");
  REQUIRE(cache.getMatchedQuantity("SECURITYID_1") == 0);
}

TEST_CASE("RemoveUnmatchedOrderShouldNotMatch", "SecurityIdTest") {
  OrderCache cache;
  const auto order1 = Order{"ID_1", "SECURITYID_1", "Buy", 1000, "User_1", "A"};

  cache.addOrder(order1);
  cache.cancelOrder("ID_1");

  const auto order2 = Order{"ID_2", "SECURITYID_1", "Sell", 1000, "User_1", "B"};
  cache.addOrder(order2);

  REQUIRE(cache.getMatchedQuantity("SECURITYID_1") == 0);
}

TEST_CASE("AddTwoSellAndRemoveOneSellShouldMatchOnlyOne", "SecurityIdTest") {
  OrderCache cache;
  const auto order2 = Order{"ID_1", "SECURITYID_1", "Sell", 1000, "User_1", "B"};
  const auto order1 = Order{"ID_2", "SECURITYID_1", "Buy", 1000, "User_1", "A"};

  const auto order3 = Order{"ID_3", "SECURITYID_2", "Sell", 1000, "User_1", "B"};
  const auto order4 = Order{"ID_4", "SECURITYID_2", "Buy", 1000, "User_1", "A"};


  cache.addOrder(order1);
  cache.addOrder(order2);
  cache.addOrder(order3);
  cache.addOrder(order4);
  REQUIRE(cache.getMatchedQuantity("SECURITYID_1") == 1000);
  REQUIRE(cache.getMatchedQuantity("SECURITYID_2") == 1000);

  cache.cancelOrder("ID_1");
  REQUIRE(cache.getMatchedQuantity("SECURITYID_1") == 0);
  cache.cancelOrder("ID_3");
  REQUIRE(cache.getMatchedQuantity("SECURITYID_2") == 0);
}

TEST_CASE("BuyOrderDistributedAccross3SellOrder", "SecurityIdTest") {
  OrderCache cache;

  const auto buyOrder = Order{"ID_1", "SECURITYID_1", "Buy", 10000, "User_1", "A"};

  const auto sellOrder1 = Order{"ID_2", "SECURITYID_1", "Sell", 3000, "User_1", "B"};
  const auto sellOrder2 = Order{"ID_3", "SECURITYID_1", "Sell", 3000, "User_4", "B"};
  const auto sellOrder3 = Order{"ID_4", "SECURITYID_1", "Sell", 3000, "User_2", "B"};



  cache.addOrder(buyOrder);
  cache.addOrder(sellOrder1);
  cache.addOrder(sellOrder2);
  cache.addOrder(sellOrder3);

  REQUIRE(cache.getMatchedQuantity("SECURITYID_1") == 9000);
}

TEST_CASE("SellOrderDistributedAccross3BuyOrder", "SecurityIdTest") {
  OrderCache cache;

  const auto sellOrder = Order{"ID_1", "SECURITYID_1", "Sell", 10000, "User_1", "A"};

  const auto buyOrder1 = Order{"ID_2", "SECURITYID_1", "Buy", 3000, "User_1", "B"};
  const auto buyOrder2 = Order{"ID_3", "SECURITYID_1", "Buy", 3000, "User_4", "B"};
  const auto buyOrder3 = Order{"ID_4", "SECURITYID_1", "Buy", 3000, "User_2", "B"};

  cache.addOrder(sellOrder);
  cache.addOrder(buyOrder1);
  cache.addOrder(buyOrder2);
  cache.addOrder(buyOrder3);

  REQUIRE(cache.getMatchedQuantity("SECURITYID_1") == 9000);
}

TEST_CASE("BuyOrderDistributedAccross3SellOrderCancelOneSellOrder", "SecurityIdTest") {
  OrderCache cache;

  const auto buyOrder = Order{"ID_1", "SECURITYID_1", "Buy", 10000, "User_1", "A"};

  const auto sellOrder1 = Order{"ID_2", "SECURITYID_1", "Sell", 3000, "User_1", "B"};
  const auto sellOrder2 = Order{"ID_3", "SECURITYID_1", "Sell", 3000, "User_4", "B"};
  const auto sellOrder3 = Order{"ID_4", "SECURITYID_1", "Sell", 3000, "User_2", "B"};



  cache.addOrder(buyOrder);
  cache.addOrder(sellOrder1);
  cache.addOrder(sellOrder2);
  cache.addOrder(sellOrder3);
  cache.cancelOrder("ID_4");

  REQUIRE(cache.getMatchedQuantity("SECURITYID_1") == 6000);
}

TEST_CASE("BuyOrderDistributedAccross3SellOrderCancelOneSellOrderAndAddAnotherOne", "SecurityIdTest") {
  OrderCache cache;

  const auto buyOrder = Order{"ID_1", "SECURITYID_1", "Buy", 10000, "User_1", "A"};

  const auto sellOrder1 = Order{"ID_2", "SECURITYID_1", "Sell", 3000, "User_1", "B"};
  const auto sellOrder2 = Order{"ID_3", "SECURITYID_1", "Sell", 3000, "User_4", "B"};
  const auto sellOrder3 = Order{"ID_4", "SECURITYID_1", "Sell", 3000, "User_2", "B"};

  cache.addOrder(buyOrder);
  cache.addOrder(sellOrder1);
  cache.addOrder(sellOrder2);
  cache.addOrder(sellOrder3);
  cache.cancelOrder("ID_4");

  REQUIRE(cache.getMatchedQuantity("SECURITYID_1") == 6000);

  const auto sellOrder4 = Order{"ID_5", "SECURITYID_1", "Sell", 6000, "User_2", "B"};
  cache.addOrder(sellOrder4);
  REQUIRE(cache.getMatchedQuantity("SECURITYID_1") == 10000);
}

TEST_CASE("RemoveAtGivenQuantity", "SecurityIdTest") {
  OrderCache cache;

  const auto buyOrder = Order{"ID_1", "SECURITYID_1", "Buy", 10000, "User_1", "A"};

  const auto sellOrder1 = Order{"ID_2", "SECURITYID_1", "Sell", 2000, "User_1", "B"};
  const auto sellOrder2 = Order{"ID_3", "SECURITYID_1", "Sell", 3000, "User_4", "B"};
  const auto sellOrder3 = Order{"ID_4", "SECURITYID_1", "Sell", 3000, "User_2", "B"};

  cache.addOrder(buyOrder);
  cache.addOrder(sellOrder1);
  cache.addOrder(sellOrder2);
  cache.addOrder(sellOrder3);

  REQUIRE(cache.getMatchedQuantity("SECURITYID_1") == 8000);
  
  cache.cancelOrdersAtQuantity("SECURITYID_1", 2000);
  REQUIRE(cache.getMatchedQuantity("SECURITYID_1") == 6000);
}

TEST_CASE("RemoveAboveGivenQuantity", "SecurityIdTest") {
  OrderCache cache;

  const auto sellOrder1 = Order{"ID_1", "SECURITYID_1", "Sell", 2000, "User_1", "A"};
  const auto sellOrder2 = Order{"ID_1", "SECURITYID_1", "Sell", 2000, "User_1", "A"};
  const auto sellOrder3 = Order{"ID_1", "SECURITYID_1", "Sell", 2000, "User_1", "A"};
  const auto sellOrder4 = Order{"ID_1", "SECURITYID_1", "Sell", 2000, "User_1", "A"};

  const auto buyOrder1 = Order{"ID_2", "SECURITYID_1", "Buy", 2000, "User_1", "B"};
  const auto buyOrder2 = Order{"ID_3", "SECURITYID_1", "Buy", 2501, "User_4", "B"};
  const auto buyOrder3 = Order{"ID_4", "SECURITYID_1", "Buy", 3000, "User_2", "B"};

  cache.addOrder(sellOrder1);
  cache.addOrder(sellOrder2);
  cache.addOrder(sellOrder3);
  cache.addOrder(sellOrder4);
  cache.addOrder(buyOrder1);
  cache.addOrder(buyOrder2);
  cache.addOrder(buyOrder3);

  REQUIRE(cache.getMatchedQuantity("SECURITYID_1") == 7501);
  
  cache.cancelOrdersAboveQuantity("SECURITYID_1", 2500);
  REQUIRE(cache.getMatchedQuantity("SECURITYID_1") == 2000);
}

TEST_CASE("AddOrderPerformanceTest", "SecurityIdTest") {
  constexpr int NUM_OF_SAMPLES = 10000;
  OrderCache cache;

  for(int i = 0; i < 10000; ++i){
    const auto sellOrder = Order{"SELL" + std::to_string(i), "SECURITYID_1", "Sell", 1000, "User_1", "COMP_" + std::to_string(i)};

    cache.addOrder(sellOrder);
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

TEST_CASE("RemoveGivenOrderIDPerformanceTest", "SecurityIdTest") {
  constexpr int NUM_OF_SAMPLES = 1000;
  OrderCache cache;

  for(int i = 0; i < 1000; ++i){
    const auto sellOrder = Order{"SELL" + std::to_string(i), "SECURITYID_1", "Sell", 1000, "User_1", "COMP_" + std::to_string(i)};

    cache.addOrder(sellOrder);
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

TEST_CASE("RemoveAboveGivenQuantityPerformanceTest", "SecurityIdTest") {
  constexpr int NUM_OF_SAMPLES = 1000;
  OrderCache cache;

  for(int i = 0; i < 1; ++i){
    const auto sellOrder = Order{"SELL" + std::to_string(i), "SECURITYID_1", "Sell", 1000, "User_1", "COMP_" + std::to_string(i)};

    cache.addOrder(sellOrder);
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