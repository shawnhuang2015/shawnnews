create vertex person(primary_id id UINT32, id UINT32, name STRING)
create vertex movie(primary_id id UINT32, id UINT32, title STRING, country STRING COMPRESS, year UINT32)
create directed edge roles(from person, to movie, role STRING)
create graph neo4j (person, movie, roles)

