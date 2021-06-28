#pragma once

#include <optional>
#include <numeric>
#include <unordered_map>
#include <map>

#include "Matcher.h"


class SecurityId{
public:
    int getMatchedQuantity() const;
    void appendOrder(const Order& order);

    void cancelOrderByOrderId(const std::string& orderID);
    void cancelOrderAtQuantity(int quantity);
    void cancelOrderAboveQuantity(int quantity);

private:

    void removeBuyOrder(const std::string& orderID);
    void removeSellOrder(const std::string& orderID);

    std::optional<Order> feedMatchers(const Order& order);
    void feedMatchersWithQueuedOrders();

    void removeFromInternalQuantityMapping(const std::string& orderID);

    std::vector<Matcher> matchers;
    std::vector<Order> unmatchedOrdersQueue;

    std::unordered_map<std::string, int>  orderIdToQuantity;
    std::multimap<int, std::string>  quantityToOrderId;
};