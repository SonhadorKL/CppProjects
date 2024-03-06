# Реализация STL-подобных контейнеров на C++

## String
Самая базовая реализация класса [string](/String/include/String.hpp).

## BigInteger
Реализация классов для работы с большими числами: [целыми](/BigInteger/include/BigInteger.h) и [рациональными](/BigInteger/include/Rational.h).

## Geometry
Класс работы с [геометрическими фигурами](/Geometry/include/Geometry.h), написанный с использованием наследования, полиморфизма и абстракций.

## STL-like Deque
Класс [Deque](/Deque/Deque.h), написанный безопасно относительно исключений и с использованием итераторов.

## STL-like List, STL-like UnorderedMap и StackAllocator
Безопасные относительно исключений [List](/ListAndMap/include/List.h) и [UnorderedMap](/ListAndMap/include/UnorderedMap.h), поддерживающие работу итераторов, аллокаторов и move-семантики. Дополнительно реализован [StackAllocator](/ListAndMap/include/StackAllocator.h), выделяющий память на стеке.

## Smart Pointers
Реализация [SharedPtr](/SmartPtr/include/SharedPtr.h) и [WeakPtr](/SmartPtr/include/WeakPtr.h).

