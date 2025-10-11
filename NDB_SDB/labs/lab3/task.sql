SELECT 
    c1.CRP_ID,
    c1.CRP_NAME,
    c1.CRP_WORTH,
    c1.CRP_WORTH + COALESCE((
        SELECT SUM(c2.CRP_WORTH)
        FROM CORP c2
        START WITH c2.CRP_PID = c1.CRP_ID
        CONNECT BY PRIOR c2.CRP_ID = c2.CRP_PID
    ), 0) AS total_worth
FROM CORP c1
ORDER BY total_worth DESC
FETCH FIRST 1 ROW ONLY;

-- SELECT *
-- FROM (
--     SELECT
--         CRP_ID,
--         CRP_NAME,
--         CRP_WORTH,
--         total_child_worth,
--         (CRP_WORTH + total_child_worth) as total_worth
--     FROM (
--         SELECT
--             parent.CRP_ID,
--             parent.CRP_NAME,
--             parent.CRP_WORTH,
--             COALESCE((
--                 SELECT SUM(child.crp_worth)
--                 FROM CORP child
--                 WHERE child.CRP_PID = parent.CRP_ID
--             ), 0) as total_child_worth
--         FROM CORP parent
--     ) org_totals
--     ORDER BY total_worth DESC
-- )
-- WHERE ROWNUM = 1;
