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
    
    void getCurrentQuantity(const std::string& secureId);

private:
    std::unordered_map<std::string, SecurityId> secuityIds;
};