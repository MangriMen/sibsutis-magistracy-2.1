CREATE OR REPLACE PROCEDURE print_sal_with_filters(
    p_sname_condition IN VARCHAR2 DEFAULT NULL,
    p_city_condition IN VARCHAR2 DEFAULT NULL,
    p_operator IN VARCHAR2 DEFAULT 'AND' -- AND or OR
)
IS
    v_sql VARCHAR2(1000);
    v_where_clause VARCHAR2(500) := '';
    v_conditions_count NUMBER := 0;
    
    v_cursor SYS_REFCURSOR;
    v_sal_row SAL%ROWTYPE;
BEGIN
    v_sql := 'SELECT SNUM, SNAME, CITY, COMM FROM SAL';

    IF p_sname_condition IS NOT NULL THEN
        v_where_clause := v_where_clause || ' AND SNAME ' || p_sname_condition;
    END IF;
    
    IF p_city_condition IS NOT NULL THEN
        v_where_clause := v_where_clause || ' ' || p_operator || ' CITY ' || p_city_condition;
    END IF;

    v_sql := v_sql || ' WHERE 1=1' || v_where_clause;

    OPEN v_cursor FOR v_sql;
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
END print_sal_with_filters;