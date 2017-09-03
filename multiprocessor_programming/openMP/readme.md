Нахождение интеграла с использованием OpenMP 
Постановка задачи.
4∫dx/(1+x^2)
                                             
Решить определенный интеграл
(1) методом трапеций с относительной 
погрешностью 0,1%. 

Задание: 
1) Получить значение  интеграла  с  использованием  общей переменной, в которую несколько нитей записывают свои результаты расчетов. 
Доступ к общей переменной обеспечить через критическую секцию.
2) Получить  значение  интеграла  с  использованием механизма reduction. 
3) Построить график зависимость ускорения S от количества нитей p (где p = 1,2,3,...,8-12).