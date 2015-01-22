# LibreOffice

Open source MS Office. Written in Java.

Forked from OpenOffice after Oracle wanted to impose stricter copyright on it.

WYSIWYG text editor, tables, image editor, database management.

How to add new spell checking language: <http://askubuntu.com/questions/72099/how-to-install-a-libreoffice-dictionary-spelling-check-thesaurus>

## Calc

Table /spreadsheet editor like Excel.

To try it out, use the `.csv` files in this directory.

## Configuration

Always show a row or column on stick rows. Useful for the header if the table is long and if there is only a single table in the spreadsheet.

- click on column that *follows* the one that should always show
- Window > Freeze

## Pivot table

<https://www.youtube.com/watch?v=7r7z8YSXG18>

Was called "Data Pilot", but finally gave in to the Microsoft terminology.

Is basically a UI for SQL `GROUP BY` queries.

Create: Select the table > Data > Pivot table. Drag and drop header fields into row, column and data.

Dragging multiple headers on rows or columns creates a hierarchy. It is not possible to open or close hierarchies quickly on the table directly: you must right click and edit the pivot layout.

To drilldown the facts, double click on the data cell. This opens a new tab with the drilldown table.
