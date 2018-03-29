apparently neither xmllint nor xmlstarlet seem to recognize camelcase
entities, like "&LT;", "&NewLine;", etc. so the file needs to be preprocessed.

the original version is downloaded to "original.entities.json", and then
processed by fixent.rb to turn entity names lowercase.
