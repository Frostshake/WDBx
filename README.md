# WDBx

Database viewer for world of warcraft.

- Opens DBC and DB2 Files.
- Supports CASC, MPQ and Native filesystem access.
- Exports to:
  - CSV
  - JSON
  - SQL

Powered by [WDBReader](https://github.com/Frostshake/WDBReader), Field names and structures from [WOWDBDefs](https://github.com/wowdev/WoWDBDefs).

![Screenshot](/screenshot.png "Screenshot")

## Building

Build with cmake and vcpkg.

```
cmake --preset=default-win-64-mixed
```

## Query editor

An *experimental* query editor is availabe from the top menu, with a syntax similiar to a SQL WHERE.

Example queries:

```
FieldName == "value"
FieldArray[0] != 123.45
FieldName IN {1,2,3}
(FieldA >= FieldB OR FieldC < FieldD)
((a == b OR c == d) AND (e == f))
```