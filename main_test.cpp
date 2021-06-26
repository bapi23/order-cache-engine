#include <gtest/gtest.h>
#include "OrderCache.h"

// Demonstrate some basic assertions.
TEST(SecurityIdTest, AddNewOrder) {
  OrderCache cache;
  const auto order1 = Order{"ID_1", "SECURITYID_1", "Buy", 1000, "User_1", "A"};
  const auto order2 = Order{"ID_2", "SECURITYID_1", "Sell", 1000, "User_1", "A"};

  cache.addOrder(order1);
  cache.addOrder(order2);
  EXPECT_EQ(1000, cache.getMatchedQuantity("SECURITYID_1"));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS();
}