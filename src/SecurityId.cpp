#include <algorithm>

#include "SecurityId.h"

int SecurityId::getMatchedQuantity() const {
    return std::accumulate(matchers.begin(), matchers.end(), 0, 
            [](int sum, const Matcher& m){ return sum + m.getMatchedQuantity();});
}

void SecurityId::appendOrder(Order order){
    const auto orderID = order.orderID;
    const int quantity = order.initialQuantity;
    if(order.side == "Sell"){
        Matcher matcher{std::move(order)};
        matchers.push_back(std::move(matcher));
    } else {
        unmatchedOrdersQueue.push_back(std::move(order));
    }
    feedMatchersWithQueuedOrders();

    quantityToOrderId.emplace(quantity, orderID);
    orderIdToQuantity.emplace(orderID, quantity);
}

void SecurityId::cancelOrderByOrderId(const std::string& orderID){
    removeBuyOrder(orderID);
    removeSellOrder(orderID);
    feedMatchersWithQueuedOrders();

    removeFromInternalQuantityMapping(orderID);
}

void SecurityId::cancelOrderAtQuantity(int quantity){
    auto range = quantityToOrderId.equal_range(quantity);
    for (auto it=range.first; it!=range.second; ++it){
        for(auto& matcher: matchers){
            removeBuyOrder(it->second);
            removeSellOrder(it->second);
            orderIdToQuantity.erase(it->second);
        }
    }
    quantityToOrderId.erase(range.first, range.second);
    feedMatchersWithQueuedOrders();
}

void SecurityId::cancelOrderAboveQuantity(int quantity){
    auto firstIt = quantityToOrderId.upper_bound(quantity);
    for (auto it=firstIt; it!=quantityToOrderId.end(); ++it){
        for(auto& matcher: matchers){
            removeBuyOrder(it->second);
            removeSellOrder(it->second);
            orderIdToQuantity.erase(it->second);
        }
    }
    quantityToOrderId.erase(firstIt, quantityToOrderId.end());
    feedMatchersWithQueuedOrders();
}

//private:
void SecurityId::removeBuyOrder(const std::string& orderID)
{
    matchedOrders.erase(orderID);
    
    auto beginToRemoveIt = std::remove_if(unmatchedOrdersQueue.begin(), unmatchedOrdersQueue.end(), 
                                [&orderID](const Order& o){ return o.orderID == orderID;});
    unmatchedOrdersQueue.erase(beginToRemoveIt, unmatchedOrdersQueue.end());
}
void SecurityId::removeSellOrder(const std::string& orderID)
{
    //Remove sell order
    auto matcherIt = std::find_if(matchers.begin(), matchers.end(), 
    [&orderID](const Matcher& matcher){ return matcher.getSellOrderID() == orderID; });

    if(matcherIt != matchers.end()){
        mergeBuyOrdersAfterRemovingSellOrder(matcherIt->retrieveAllOrders());
        matchers.erase(matcherIt);
    }
}

void SecurityId::mergeBuyOrdersAfterRemovingSellOrder(const std::vector<Order*>& orders){
    for(const auto& order: orders){
        auto foundIt = std::find_if(unmatchedOrdersQueue.begin(), unmatchedOrdersQueue.end(), 
        [&order](const Order& o){ return order->orderID == o.orderID;});
        if(foundIt != unmatchedOrdersQueue.end()){
            foundIt->quantity += order->quantity;
        } else {
            auto findIt = matchedOrders.find(order->orderID);
            Order order = std::move(findIt->second);
            matchedOrders.erase(findIt);
            unmatchedOrdersQueue.push_back(std::move(order));
        }
    }
}

std::optional<Order> SecurityId::feedMatchers(Order order){
    const auto orderId = order.orderID;
    std::optional<Order> currentOrder(std::move(order));
    for(auto& matcher: matchers){
        if(matcher.hasRemainingSellQuantity() && matcher.sellOrderCompanyName() != currentOrder->companyName){
            auto it = matchedOrders.emplace(orderId, std::move(*currentOrder));
            currentOrder = matcher.addBuyOrder(&(it->second));
            if(!currentOrder){
                return std::nullopt;
            }
        }
    }
    return currentOrder;
}

void SecurityId::feedMatchersWithQueuedOrders(){
    //check if we can handle now existing buy orders:
    std::vector<Order> resultQueue;
    for(auto& order: unmatchedOrdersQueue){
        auto remainingOrder = feedMatchers(std::move(order));
        if(remainingOrder){
            resultQueue.push_back(std::move(*remainingOrder));
        }
    }
    unmatchedOrdersQueue = std::move(resultQueue);
}

void SecurityId::removeFromInternalQuantityMapping(const std::string& orderID){
    auto found_quantity = orderIdToQuantity.find(orderID);
    assert(found_quantity != orderIdToQuantity.end());

    auto range = quantityToOrderId.equal_range(found_quantity->second);
    std::vector<std::multimap<int,std::string>::iterator> toErase;
    for (auto it=range.first; it!=range.second; ++it){
        if(it->second == orderID){
            toErase.push_back(it);
        }
    }
    for(const auto& iter: toErase){
        quantityToOrderId.erase(iter);
    }
}