#!/bin/csh -f
#
set dicts = "mmcif_pdbx_v40.dic mmcif_pdbx_v5_next.dic mmcif_pdbx_v50.dic mmcif_std.dic mmcif_ddl.dic mmcif_em.dic mmcif_img.dic  mmcif_nmr-star.dic mmcif_sas.dic mmcif_biosync.dic mmcif_ccp4.dic  mmcif_ma.dic mmcif_sym.dic mmcif_nef.dic mmcif_ihm.dic mmcif_ndb_ntc.dic"
#
setenv DICT_PATH    /var/www/mmcif_website/mmcif/dictionaries
setenv SWISH_PATH   /var/www/mmcif_website/mmcif_cgi/swish
#
rm -f ./indexes/i.*
foreach d ($dicts)
  /usr/bin/swish-e \
     -S fs \
     -c $SWISH_PATH/swish.conf \
     -f $SWISH_PATH/indexes/i.$d \
     -i $DICT_PATH/$d/Categories $DICT_PATH/$d/Items
end

# ---
#  /opt/local/bin/swish-e \
#     -S fs \
#     -c /www/cgi-ontologies/mmcif/swish/swish.conf \
#     -f /www/cgi-ontologies/mmcif/swish/indexes/i.general \
#     -i /www/docs-ontologies/mmcif/index.html \
#        /www/docs-ontologies/mmcif/mmcif-early \
#        /www/docs-ontologies/mmcif/pubs \
#        /www/docs-ontologies/mmcif/sg-data
#
##

