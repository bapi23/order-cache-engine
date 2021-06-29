#include "Order.h"
#include "Matcher.h"

Order::Order(const std::string& orderID,
        const std::string& securityID,
        const std::string& side,
        int quantity,
        const std::string& userID,
        const std::string& companyName):
        orderID(orderID),
        securityID(securityID),
        side(side),
        initialQuantity(quantity),
        quantity(quantity),
        userID(userID),
        companyName(companyName),
        matcher(nullptr)
        {}

Order Order::clone(){
    Order order{orderID, securityID, side, quantity, userID, companyName};
    order.initialQuantity = initialQuantity;
    order.matcher = nullptr;
    return order;
}
Order& Order::operator=(Order&& order){
    this->orderID = std::move(order.orderID);
    this->securityID = std::move(order.securityID);
    this->side = std::move(order.side);
    this->initialQuantity = order.initialQuantity;
    this->quantity = std::move(order.quantity);
    this->userID = std::move(order.userID);
    this->companyName = std::move(order.companyName);
    this->matcher = order.matcher;
    return *this;
}
Order::Order(Order&& order){
    this->orderID = std::move(order.orderID);
    this->securityID = std::move(order.securityID);
    this->side = std::move(order.side);
    this->initialQuantity = order.initialQuantity;
    this->quantity = std::move(order.quantity);
    this->userID = std::move(order.userID);
    this->companyName = std::move(order.companyName);
    this->matcher = order.matcher;
}

void Order::setMatcher(Matcher* matcher){
    this->matcher = matcher;
}

void Order::unsubscribeFromMatcher(Matcher* matcher){
    matcher = nullptr;
}

Order::~Order(){
    if(matcher){
        matcher->removeBuyOrder(orderID);
    }
}