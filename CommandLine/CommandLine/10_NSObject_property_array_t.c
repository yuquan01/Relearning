#pragma mark  ------------------------定义
//property_array_t 的定义如下
//
class property_array_t :
public list_array_tt<property_t, property_list_t>
{
    typedef list_array_tt<property_t, property_list_t> Super;
    
public:
    property_array_t() : Super() { }
    property_array_t(property_list_t *l) : Super(l) { }
    
    property_array_t duplicate() {
        return Super::duplicate<property_array_t>();
    }
};
