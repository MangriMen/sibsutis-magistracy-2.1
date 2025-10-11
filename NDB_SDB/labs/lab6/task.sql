-- -------- CASE 1 --------
-- Атака через p_sname_condition
BEGIN
    print_sal_with_filters(p_sname_condition => '= ''ANY'' OR 1=1--');
END;

-- Результат: выводятся все записи таблицы SAL
-- 1001      Peel           London         .12
-- 1002      Serres         San Jose       .13
-- 1004      Motica         London         .11
-- 1007      Rifkin         Barcelona      .15
-- 1003      Axelrod        New York       .1

-- -------- CASE 2 --------
-- Получение данных из других таблиц
BEGIN
    print_sal_with_filters(
        p_sname_condition => '= ''test'' UNION SELECT EMPLOYEE_ID, FIRST_NAME, LAST_NAME, null FROM EMPLOYEES--',
        p_operator => 'OR'
    );
END;

-- Результат: выводятся записи из таблицы EMPLOYEES
-- 100       Steven         King           
-- 101       Neena          Kochhar        
-- 102       Lex            De Haan        
-- 103       Alexander      Hunold         
-- 104       Bruce          Ernst          
-- 105       David          Austin         
-- 106       Valli          Pataballa      
-- 107       Diana          Lorentz        
-- 108       Nancy          Greenberg      
-- 109       Daniel         Faviet         
-- 110       John           Chen           
-- 111       Ismael         Sciarra  

-- -------- CASE 3 --------
-- Использование подзапроса для извлечения чувствительной информации
BEGIN
    print_sal_with_filters(
        p_sname_condition => '= ''x'' OR EXISTS (SELECT * FROM CUST WHERE CNAME LIKE ''Hoffman'')--'
    );
END;

-- Результат: выводятся записи, а значит в таблице CUST есть заказчик с CNAME = Hoffman
-- 1001      Peel           London         .12
-- 1002      Serres         San Jose       .13
-- 1004      Motica         London         .11
-- 1007      Rifkin         Barcelona      .15
-- 1003      Axelrod        New York       .1

-- Основные методы защиты:
-- Использование связываемых переменных (bind variables)
-- Валидация и ограничение входных параметров
-- Принцип минимальных привилегий
-- Использование статических запросов
-- Экранирование специальных символов

-- Исправленная версия
CREATE OR REPLACE PROCEDURE print_sal_with_filters_safe(
    p_sname IN VARCHAR2 DEFAULT NULL,
    p_city IN VARCHAR2 DEFAULT NULL,
    p_operator IN VARCHAR2 DEFAULT 'AND'
)
IS
    v_sql VARCHAR2(1000);
    v_where_clause VARCHAR2(500) := '';
    v_conditions_count NUMBER := 0;
    
    v_cursor SYS_REFCURSOR;
    v_sal_row SAL%ROWTYPE;
    
    -- Константы для валидации
    C_VALID_OPERATORS CONSTANT VARCHAR2(10) := 'AND,OR';
    C_MAX_NAME_LENGTH CONSTANT NUMBER := 50;
BEGIN
    -- Валидация оператора
    IF p_operator NOT IN ('AND', 'OR') THEN
        RAISE_APPLICATION_ERROR(-20001, 'Invalid operator. Only AND or OR allowed.');
    END IF;
    
    -- Валидация длины строковых параметров
    IF (p_sname IS NOT NULL AND LENGTH(p_sname) > C_MAX_NAME_LENGTH) OR
       (p_city IS NOT NULL AND LENGTH(p_city) > C_MAX_NAME_LENGTH) THEN
        RAISE_APPLICATION_ERROR(-20002, 'Input parameter too long.');
    END IF;
    
    -- Построение безопасного запроса с связываемыми переменными
    v_sql := 'SELECT SNUM, SNAME, CITY, COMM FROM SAL WHERE 1=1';
    
    IF p_sname IS NOT NULL THEN
        v_sql := v_sql || ' ' || p_operator || ' SNAME = :sname';
    END IF;
    
    IF p_city IS NOT NULL THEN
        v_sql := v_sql || ' ' || p_operator || ' CITY = :city';
    END IF;
    
    -- Открытие курсора с связываемыми переменными
    IF p_sname IS NOT NULL AND p_city IS NOT NULL THEN
        OPEN v_cursor FOR v_sql USING p_sname, p_city;
    ELSIF p_sname IS NOT NULL THEN
        OPEN v_cursor FOR v_sql USING p_sname;
    ELSIF p_city IS NOT NULL THEN
        OPEN v_cursor FOR v_sql USING p_city;
    ELSE
        OPEN v_cursor FOR v_sql;
    END IF;
    
    -- Вывод результатов
    LOOP
        FETCH v_cursor INTO v_sal_row;
        EXIT WHEN v_cursor%NOTFOUND;
        
        DBMS_OUTPUT.PUT_LINE(
            RPAD(v_sal_row.snum, 10) || 
            RPAD(v_sal_row.sname, 15) || 
            RPAD(v_sal_row.city, 15) || 
            v_sal_row.comm
        );
    END LOOP;
    CLOSE v_cursor;

EXCEPTION
    WHEN OTHERS THEN
        DBMS_OUTPUT.PUT_LINE('Error: ' || SQLERRM);
        IF v_cursor%ISOPEN THEN
            CLOSE v_cursor;
        END IF;
END print_sal_with_filters_safe;

-- Теперь повторим все кейсы
-- -------- CASE 1 --------
BEGIN
    print_sal_with_filters_safe(p_sname_condition => '= ''ANY'' OR 1=1--');
END;

-- Результат:
-- Error at line 2/5: ORA-06550: line 2, column 5:
-- PLS-00306: wrong number or types of arguments in call to 'PRINT_SAL_WITH_FILTERS_SAFE'
-- ORA-06512: at "SYS.WWV_DBMS_SQL_APEX_240200", line 803
-- ORA-06550: line 2, column 5:
-- PL/SQL: Statement ignored


-- 1. BEGIN
-- 2.     print_sal_with_filters_safe(p_sname_condition => '= ''ANY'' OR 1=1--');
-- 3. END;

-- -------- CASE 2 --------
BEGIN
    print_sal_with_filters_safe(
        p_sname_condition => '= ''test'' UNION SELECT EMPLOYEE_ID, FIRST_NAME, LAST_NAME, null FROM EMPLOYEES--',
        p_operator => 'OR'
    );
END;

-- Результат:
-- Error at line 2/5: ORA-06550: line 2, column 5:
-- PLS-00306: wrong number or types of arguments in call to 'PRINT_SAL_WITH_FILTERS_SAFE'
-- ORA-06512: at "SYS.WWV_DBMS_SQL_APEX_240200", line 803
-- ORA-06550: line 2, column 5:
-- PL/SQL: Statement ignored


-- 1. BEGIN
-- 2.     print_sal_with_filters_safe(
-- 3.         p_sname_condition => '= ''test'' UNION SELECT EMPLOYEE_ID, FIRST_NAME, LAST_NAME, null FROM EMPLOYEES--',
-- 4.         p_operator => 'OR'

-- -------- CASE 3 --------
BEGIN
    print_sal_with_filters_safe(
        p_sname_condition => '= ''x'' OR EXISTS (SELECT * FROM CUST WHERE CNAME LIKE ''Hoffman'')--'
    );
END;

-- Результат:
-- Error at line 2/5: ORA-06550: line 2, column 5:
-- PLS-00306: wrong number or types of arguments in call to 'PRINT_SAL_WITH_FILTERS_SAFE'
-- ORA-06512: at "SYS.WWV_DBMS_SQL_APEX_240200", line 803
-- ORA-06550: line 2, column 5:
-- PL/SQL: Statement ignored


-- 1. BEGIN
-- 2.     print_sal_with_filters_safe(
-- 3.         p_sname_condition => '= ''x'' OR EXISTS (SELECT * FROM CUST WHERE CNAME LIKE ''Hoffman'')--'
-- 4.     );