#include <gtest/gtest.h>
#include "OrderCache.h"

// Demonstrate some basic assertions.
TEST(SecurityIdTest, AddTwoOrdersShouldMatch) {
  OrderCache cache;
  const auto order1 = Order{"ID_1", "SECURITYID_1", "Buy", 1000, "User_1", "A"};
  const auto order2 = Order{"ID_2", "SECURITYID_1", "Sell", 1000, "User_1", "B"};

  cache.addOrder(order1);
  cache.addOrder(order2);
  EXPECT_EQ(1000, cache.getMatchedQuantity("SECURITYID_1"));
}

TEST(SecurityIdTest, GivenExampleTest) {
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
  EXPECT_EQ(2700, cache.getMatchedQuantity("US5422358DA3"));
  EXPECT_EQ(0, cache.getMatchedQuantity("US19635GY645"));
  EXPECT_EQ(0, cache.getMatchedQuantity("US9128473801"));
}

TEST(SecurityIdTest, AddTwoOrdersAndRemoveOneShouldNotMatch) {
  OrderCache cache;
  const auto order1 = Order{"ID_1", "SECURITYID_1", "Buy", 1000, "User_1", "A"};
  const auto order2 = Order{"ID_2", "SECURITYID_1", "Sell", 1000, "User_1", "B"};

  cache.addOrder(order1);
  cache.addOrder(order2);
  EXPECT_EQ(1000, cache.getMatchedQuantity("SECURITYID_1"));

  cache.cancelOrder("ID_1");
  EXPECT_EQ(0, cache.getMatchedQuantity("SECURITYID_1"));
}

TEST(SecurityIdTest, AddTwoSellAndRemoveOneSellShouldMatchOnlyOne) {
  OrderCache cache;
  const auto order2 = Order{"ID_1", "SECURITYID_1", "Sell", 1000, "User_1", "B"};
  const auto order1 = Order{"ID_2", "SECURITYID_1", "Buy", 1000, "User_1", "A"};

  const auto order3 = Order{"ID_3", "SECURITYID_2", "Sell", 1000, "User_1", "B"};
  const auto order4 = Order{"ID_4", "SECURITYID_2", "Buy", 1000, "User_1", "A"};


  cache.addOrder(order1);
  cache.addOrder(order2);
  cache.addOrder(order3);
  cache.addOrder(order4);
  EXPECT_EQ(1000, cache.getMatchedQuantity("SECURITYID_1"));
  EXPECT_EQ(1000, cache.getMatchedQuantity("SECURITYID_2"));

  cache.cancelOrder("ID_1");
  EXPECT_EQ(0, cache.getMatchedQuantity("SECURITYID_1"));
  cache.cancelOrder("ID_3");
  EXPECT_EQ(0, cache.getMatchedQuantity("SECURITYID_2"));
}

TEST(SecurityIdTest, BuyOrderDistributedAccross3SellOrder) {
  OrderCache cache;

  const auto buyOrder = Order{"ID_1", "SECURITYID_1", "Buy", 10000, "User_1", "A"};

  const auto sellOrder1 = Order{"ID_2", "SECURITYID_1", "Sell", 3000, "User_1", "B"};
  const auto sellOrder2 = Order{"ID_3", "SECURITYID_1", "Sell", 3000, "User_4", "B"};
  const auto sellOrder3 = Order{"ID_4", "SECURITYID_1", "Sell", 3000, "User_2", "B"};



  cache.addOrder(buyOrder);
  cache.addOrder(sellOrder1);
  cache.addOrder(sellOrder2);
  cache.addOrder(sellOrder3);

  EXPECT_EQ(9000, cache.getMatchedQuantity("SECURITYID_1"));
}

TEST(SecurityIdTest, SellOrderDistributedAccross3BuyOrder) {
  OrderCache cache;

  const auto sellOrder = Order{"ID_1", "SECURITYID_1", "Sell", 10000, "User_1", "A"};

  const auto buyOrder1 = Order{"ID_2", "SECURITYID_1", "Buy", 3000, "User_1", "B"};
  const auto buyOrder2 = Order{"ID_3", "SECURITYID_1", "Buy", 3000, "User_4", "B"};
  const auto buyOrder3 = Order{"ID_4", "SECURITYID_1", "Buy", 3000, "User_2", "B"};

  cache.addOrder(sellOrder);
  cache.addOrder(buyOrder1);
  cache.addOrder(buyOrder2);
  cache.addOrder(buyOrder3);

  EXPECT_EQ(9000, cache.getMatchedQuantity("SECURITYID_1"));
}

TEST(SecurityIdTest, BuyOrderDistributedAccross3SellOrderCancelOneSellOrder) {
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

  EXPECT_EQ(6000, cache.getMatchedQuantity("SECURITYID_1"));
}

TEST(SecurityIdTest, BuyOrderDistributedAccross3SellOrderCancelOneSellOrderAndAddAnotherOne) {
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

  EXPECT_EQ(6000, cache.getMatchedQuantity("SECURITYID_1"));

  const auto sellOrder4 = Order{"ID_5", "SECURITYID_1", "Sell", 6000, "User_2", "B"};
  cache.addOrder(sellOrder4);
  EXPECT_EQ(10000, cache.getMatchedQuantity("SECURITYID_1"));
}

TEST(SecurityIdTest, RemoveAtGivenQuantity) {
  OrderCache cache;

  const auto buyOrder = Order{"ID_1", "SECURITYID_1", "Buy", 10000, "User_1", "A"};

  const auto sellOrder1 = Order{"ID_2", "SECURITYID_1", "Sell", 2000, "User_1", "B"};
  const auto sellOrder2 = Order{"ID_3", "SECURITYID_1", "Sell", 3000, "User_4", "B"};
  const auto sellOrder3 = Order{"ID_4", "SECURITYID_1", "Sell", 3000, "User_2", "B"};

  cache.addOrder(buyOrder);
  cache.addOrder(sellOrder1);
  cache.addOrder(sellOrder2);
  cache.addOrder(sellOrder3);

  EXPECT_EQ(8000, cache.getMatchedQuantity("SECURITYID_1"));
  
  cache.cancelOrdersAtQuantity("SECURITYID_1", 2000);
  EXPECT_EQ(6000, cache.getMatchedQuantity("SECURITYID_1"));
}

TEST(SecurityIdTest, RemoveAboveGivenQuantity) {
  OrderCache cache;

  const auto buyOrder = Order{"ID_1", "SECURITYID_1", "Buy", 10000, "User_1", "A"};

  const auto sellOrder1 = Order{"ID_2", "SECURITYID_1", "Sell", 2000, "User_1", "B"};
  const auto sellOrder2 = Order{"ID_3", "SECURITYID_1", "Sell", 2501, "User_4", "B"};
  const auto sellOrder3 = Order{"ID_4", "SECURITYID_1", "Sell", 3000, "User_2", "B"};

  cache.addOrder(buyOrder);
  cache.addOrder(sellOrder1);
  cache.addOrder(sellOrder2);
  cache.addOrder(sellOrder3);

  EXPECT_EQ(7501, cache.getMatchedQuantity("SECURITYID_1"));
  
  cache.cancelOrdersAboveQuantity("SECURITYID_1", 2500);
  EXPECT_EQ(2000, cache.getMatchedQuantity("SECURITYID_1"));
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS();
}