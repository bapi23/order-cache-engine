#pragma once

#include <cassert>

#include <string>

class Matcher;
class Order{
public:
    Order(const std::string& orderID,
          const std::string& securityID,
          const std::string& side,
          int quantity,
          const std::string& userID,
          const std::string& companyName);

    std::string orderID;     // e.g  "ID1" (unique id for order)
    std::string securityID;  // e.g. "US9128473801"
    std::string side;        // e.g. “Buy” or “Sell” 
    int initialQuantity;     // Initial quantity used for removing orders
    int quantity;            // e.g. 120000
    std::string userID;      // e.g. “John Smith” 
    std::string companyName; // e.g. “Hedge Fund A” (company of User ID)

    Order clone();
    Order& operator=(Order&& order);
    Order(Order&& order);

    Order& operator=(const Order& order) = delete;
    Order(const Order& order) = delete;

    void setMatcher(Matcher* matcher);

    void unsubscribeFromMatcher(Matcher* matcher);

    ~Order();

private:
    Matcher* matcher;
};