#pragma once

#include <string>
#include <unordered_map>

#include "Order.hpp"
#include "SecurityId.h"

class OrderCache{
public:
    void addOrder(const Order& order);
    void cancelOrder(const std::string& orderId);
    void cancelOrdersForUser(const std::string& uderId);
    void cancelOrdersAboveQuantity(const std::string& secureId, int quantity);
    void cancelOrdersAtQuantity(const std::string& secureId, int quantity);
    
    int getMatchedQuantity(const std::string& secureId) const ;

private:
    std::unordered_map<std::string, SecurityId> securityIds;
    std::unordered_map<std::string, std::string> orderIdToSecurityId;
    std::unordered_map<std::string, std::vector<std::string>> userIdToOrderIds;
};