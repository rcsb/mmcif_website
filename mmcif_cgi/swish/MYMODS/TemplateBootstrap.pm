#
# File: TemplateBootstrap.pm
# Date: 12-Sep-2013 Jdw
# 
# Bootstrap CSS adaptation of SWISH results output -- 
# 
# Depends on configuration in ../.swishcgi.conf
#
# Updates --  
#   6-Oct-2013 jdw   added form to results -- 
#                    catch null result conditions -
#
#=====================================================================
# These routines format the HTML output.
#    $Id: TemplateDefault.pm 1904 2007-02-07 18:51:33Z moseley $
#=====================================================================
package MYMODS::TemplateBootstrap;
use strict;

use CGI;

sub show_template {
    my ( $class, $template_params, $results ) = @_;


    my $q = $results->CGI;

    my $output =  $q->header . page_header( $results );

    # Show form at top always
    $output .= show_form( $results );


    if ( $results->results ) {
        $output .=  results_header( $results );
        $output .=  show_result( $results, $_ ) for @{ $results->results };
    } else {
	$output .=  "<br />No results for this query...<br />"
    }

    # Form after results (or at top if no results)
    #$output .= show_form( $results );

    $output .= footer( $results );

    print $output;

}

#=====================================================================
# This generates the header

sub page_header {
    my $results = shift;
    my $title = $results->config('title') || 'Search our site with Swish-e';
    my $message = $results->errstr;
    my $my_include_path = $results->config('my_include_path');

    $message = $message
        ? qq[<br><font color="red">$message</font>]
        : '' ;


    my $html_title = $results->results
        ? ( $results->navigation('hits')
            . ' Results for ['
            . CGI::escapeHTML( $results->{query_simple} )
            . ']'
           )

        : ( $results->errstr || $title );

    my $default_logo = '<a href="http://swish-e.org"><img border="0" alt="Swish-e home page" src="http://swish-e.org/grahipcs/swish-e.gif"></a> ' ;


    my $logo = $results->config('on_intranet')
               ? ''
               : $results->config('logo') || $default_logo;

    my $temp_file = join "/", $my_include_path, "head_common_bs.html";
    open FILE,  $temp_file or die "Couldn't open file: $!"; 
    my $temp_content_1  = join("", <FILE>); 
    close FILE;

    $temp_file = join "/", $my_include_path, "page_header_bs.html";
    open FILE,  $temp_file or die "Couldn't open file: $!"; 
    my $temp_content_2  = join("", <FILE>); 
    close FILE;



    return <<EOF;
<!DOCTYPE html>
<html lang="en">
  <head>

    $temp_content_1

   <title>$title</title>    
  </head>

  <body>
    <div id="wrap">
      <!-- common page navigation -->
      <!--#XXXinclude virtual="/includes/page_header_bs.html"-->

      $temp_content_2

       <div class="container">
          <div class="row">
            <h2>PDBx/mmCIF Search Results</h2>
          </div>

<!-- End  file page_header_bs.html -->
<!---         $logo$title $message  -->
EOF
}

#=====================================================================
# This generates the form
#
#   Pass:
#       $results hash

sub show_form {

    my $results = shift;
    my $q = $results->{q};


    my $query = $q->param('query') || '';

    $query = CGI::escapeHTML( $query );  # May contain quotes


    my $si = $q->param('si');
    my $sel0 = '';     
    my $sel1 = '';     
    my $sel2 = '';     
    my $sel3 = '';     
    my $sel4 = '';
    my $sel5 = '';     
    my $sel6 = '';     
    my $sel7 = '';     
    my $sel8 = '';     
    my $sel9 = '';  
    my $sel10 = '';
    my $sel11 = '';
    my $sel12 = '';
    my $sel13 = '';
    my $sel14 = '';
    my $sel15 = '';
    if  ( $si eq '0' ) {
        $sel0 = 'selected';
    }  elsif  ( $si eq '1' ) {
        $sel1 = 'selected';
    }    elsif  ( $si eq '2' ) {
        $sel2 = 'selected';
    }    elsif  ( $si eq '3' ) {
        $sel3 = 'selected';
    }   elsif  ( $si eq '4' ) {
        $sel4 = 'selected';
    }    elsif  ( $si eq '6' ) {
        $sel6 = 'selected';
    }    elsif  ( $si eq '7' ) {
        $sel7 = 'selected';
    }    elsif  ( $si eq '7' ) {
        $sel7 = 'selected';
    }    elsif  ( $si eq '8' ) {
        $sel8 = 'selected';
    }    elsif  ( $si eq '9' ) {
        $sel9 = 'selected';
    }    elsif  ( $si eq '10' ) {
        $sel10 = 'selected';
    }    elsif  ( $si eq '11' ) {
        $sel11 = 'selected';
    }    elsif  ( $si eq '12' ) {
        $sel12 = 'selected';
    }    elsif  ( $si eq '13' ) {
        $sel13 = 'selected';
    }    elsif  ( $si eq '14' ) {
        $sel14 = 'selected';
    }    elsif  ( $si eq '15' ) {
        $sel15 = 'selected';
    }


    # Here's some form components
    
    my $meta_select_list    = get_meta_name_limits( $results );
    my $sorts               = get_sort_select_list( $results );
    my $select_index        = get_index_select_list( $results );
    my $limit_select        = get_limit_select( $results );
    
    my $date_ranges_select  = $results->get_date_ranges;

    my $form = $q->script_name;

    my $advanced_link = qq[<small><a href="$form">advanced form</a></small>]; 

    my $advanced_form = $q->param('brief')
                        ? $advanced_link
                        : <<EOF;
        $meta_select_list
        $sorts
        $select_index
        $limit_select
        $date_ranges_select
EOF

    my $extra = $results->config('extra_fields');
    my $hidden = !$extra ? ''
                 : join "\n", map { $q->hidden($_) } @$extra; 


    
    return <<EOF;
    <!--
    <form method="get" action="$form" enctype="application/x-www-form-urlencoded" class="form">
        <input maxlength="200" value="$query" size="32" type="text" name="query"/>
        $hidden
        <input value="Search!" type="submit" name="submit"/><br>

        $advanced_form
    </form>
     -->
	<div class="row">
	  <br /> 
	</div>
	<div class="row">
	    <form id="searchSite" action="/cgi-bin/swish/swish.cgi" method="get"  class="form-inline" role="form">

              <div class="form-group col-md-4 col-md-offset-2">
                <input type="text" placeholder="Search" class="form-control" name="query" value="$query">
              </div>



              <div class="form-group">
                <select name="si" class="form-control" placeholder="Select dictionary to search">
                  <option $sel0 value="0">Current PDBx/mmCIF Exchange Dictionary V5</option>
                  <option $sel1 value="1">Development PDBx/mmCIF Exchange Dictionary V5</option>
                  <option $sel2 value="2">Previous PDBx/mmCIF Exchange Dictionary V4</option>
                  <option $sel3 value="3">Original mmCIF Dictionary</option>
                  <option $sel4 value="4">Dictionary Definition Language Dictionary (DDL) </option>
                  <option $sel5 value="5">3DEM Dictionary</option>
                  <option $sel6 value="6">Image Dictionary</option>
                  <option $sel7 value="7">NMRIF/NMR-STAR Dictionary</option>
                  <option $sel8 value="8">Small-Angle Scattering Dictionary</option>
                  <option $sel9 value="9">BIOSYNC Dictionary</option>
                  <option $sel10 value="10">CCP4 Dictionary</option>
                  <option $sel11 value="11">ModelCIF Dictionary</option>
                  <option $sel12 value="12">Symmetry Dictionary</option>
                  <option $sel13 value="13">Integrative/Hybrid Methods (IHM)</option>
	          <option $sel14 value="14">NDB NTC Dictionary</option>
                  <option $sel15 value="15">wwPDB Validation Dictionary</option>	
                </select>
              </div>

              <button type="submit" class="btn btn-wwpdb-green">
                <span class="glyphicon glyphicon-search"></span></a>
              </button>

              <input type="hidden" name="sort" value="swishrank" />
            </form>
	</div>
	<div class="row">
	<hr />
	</div>


EOF
}


#=====================================================================
# This routine creates the results header display
# and navigation bar
#
#
#

sub results_header {

    my $results = shift;
    my $config = $results->{config};
    my $q = $results->{q};



    my $swr = $results->header('removed stopwords');
    my $stopwords = '';


    if ( $swr && ref $swr eq 'ARRAY' ) {
        $stopwords = @$swr > 1
        ? join( ', ', map { "<b>$_</b>" } @$swr ) . ' are very common words and were not included in your search'
        : join( ', ', map { "<b>$_</b>" } @$swr ) . ' is a very common word and was not included in your search';
    }

    my $limits = '';

    #  Ok, this is ugly.


    if ( $results->{DateRanges_time_low} && $results->{DateRanges_time_high} ) {
        my $low = scalar localtime $results->{DateRanges_time_low};
        my $high = scalar localtime $results->{DateRanges_time_high};
        $limits = <<EOF;
        <tr>
            <td colspan=2>
                <font size="-2" face="Geneva, Arial, Helvetica, San-Serif">
                &nbsp;Results limited to dates $low to $high
                </font>
            </td>
        </tr>
EOF
    }

    my $query_href = $results->{query_href};
    my $query_simple = CGI::escapeHTML( $results->{query_simple} );
    my $pages       = $results->navigation('pages');

    my $prev        = $results->navigation('prev');
    my $prev_count  = $results->navigation('prev_count');
    my $next        = $results->navigation('next');
    my $next_count  = $results->navigation('next_count');

    my $hits        = $results->navigation('hits');
    my $from        = $results->navigation('from');
    my $to          = $results->navigation('to');

    my $run_time    = $results->navigation('run_time');
    my $search_time = $results->navigation('search_time');





    my $links = '';

    $links .= '<font size="-1" face="Geneva, Arial, Helvetica, San-Serif">&nbsp;Page:</font>' . $pages
        if $pages;

    $links .= qq[ <a href="$query_href&amp;start=$prev">Previous $prev_count</a>]
        if $prev_count;

    $links .= qq[ <a href="$query_href&amp;start=$next">Next $next_count</a>]
        if $next_count;


    # Save for the bottom of the screen.
    $results->{LINKS} = $links;

    $links = qq[<tr><td colspan="2" bgcolor="#EEEEEE">$links</td></tr>] if $links;

    $query_simple = $query_simple
        ? "&nbsp;Results for <b>$query_simple</b>"
        : '';



    return <<EOF;

    <table cellpadding=0 cellspacing=0 border=0 width="100%">
        <tr>
            <td height=20 bgcolor="#99c49b">
                <font size="-1" face="Geneva, Arial, Helvetica, San-Serif">
                $query_simple
                &nbsp; $from to $to of $hits results.
                </font>
            </td>
            <td align=right bgcolor="#99c49b">
                <font size="-2" face="Geneva, Arial, Helvetica, San-Serif">
                Run time: $run_time |
                Search time: $search_time &nbsp; &nbsp;
                </font>
            </td>
        </tr>

        $links
        $limits
        $stopwords

    </table>

EOF

}

#=====================================================================
# This routine formats a single result
#
#
sub show_result {
    my ($results, $this_result ) = @_;

    my $conf = $results->{conf};

    my $DocTitle = $results->config('title_property') || 'swishtitle';


    my $title = $this_result->{$DocTitle} || $this_result->{swishdocpath} || '?';

    my $name_labels = $results->config('name_labels');



    # The the properties to display

    my $props = '';

    my $display_props = $results->config('display_props');
    if ( $display_props ) {


        $props = join "\n",
            '<br><table cellpadding="0" cellspacing="0">',
            map ( {
                '<tr><td><small>'
                . ( $name_labels->{$_} || $_ )
                . ':</small></td><td><small> '
                . '<b>'
                . ( defined $this_result->{$_} ?  $this_result->{$_} : '' ) 
                . '</b>'
                . '</small></td></tr>'
                 }   @$display_props
            ),
            '</table>';
    }


    my $description_prop = $results->config('description_prop');

    my $description = '';
    if ( $description_prop ) {
        $description = $this_result->{ $description_prop } || '';
    }


    return <<EOF;
    <dl>
        <dt>$this_result->{swishreccount} <a href="$this_result->{swishdocpath_href}">$title</a> <small>-- rank: <b>$this_result->{swishrank}</b></small></dt>
        <dd>$description

        $props
        </dd>
    </dl>

EOF

}

#=====================================================================
# This is displayed on the bottom of every page
#
#


sub footer {
    my $results = shift;

    my $mod_perl = $ENV{MOD_PERL}
               ? '<br><small>Response brought to you by <a href="http://perl.apache.org"><em>mod_perl</em></a></small>'
               : '';

    my $valid_html_logo = $results->config('on_intranet')
               ? ''
               : '<p><a href="http://validator.w3.org/check/referer"><img border="0" src="http://www.w3.org/Icons/valid-html401" alt="Valid HTML 4.01!" height="31" width="88"></a></p>';


    my $links = $results->{LINKS} || '';

    my $my_include_path = $results->config('my_include_path');

    my $temp_file = join "/", $my_include_path,"page_javascript_bs.html";
    open FILE,  $temp_file or die "Couldn't open file: $!"; 
    my $temp_content_1  = join("", <FILE>); 
    close FILE;

    $temp_file =  join "/", $my_include_path, "page_footer_bs.html";
    open FILE,  $temp_file or die "Couldn't open file: $!"; 
    my $temp_content_2  = join("", <FILE>); 
    close FILE;

    return <<EOF;


    $links
    <hr>
    </div> <!-- end of top container -->
    </div> <!-- end wrap -->
    $temp_content_1
    $temp_content_2
    <!--#XXinclude virtual="/includes/page_javascript_bs.html"-->
    <!--#XXinclude virtual="/includes/page_footer_bs.html"-->
  </body>
</html>
EOF
}

#==================================================================
#  Form setup for sorts and metas
#
#  This could be methods of $results object
#  (and then available for Template-Toolkit)
#  But that's too much HTML in the object, perhaps.
#
#
#==================================================================

sub get_meta_name_limits {
    my ( $results ) = @_;

    my $metanames = $results->config('metanames');
    return '' unless $metanames;


    my $name_labels = $results->config('name_labels');
    my $q = $results->CGI;


    return join "\n",
        'Limit search to:',
        $q->radio_group(
            -name   =>'metaname',
            -values => $metanames,
            -default=>$metanames->[0],
            -labels =>$name_labels
        ),
        '<br>';
}

sub get_sort_select_list {
    my ( $results ) = @_;

    my $sort_metas = $results->config('sorts');
    return '' unless $sort_metas;

    
    my $name_labels = $results->config('name_labels');
    my $q = $results->CGI;



    return join "\n",
        'Sort by:',
        $q->popup_menu(
            -name   =>'sort',
            -values => $sort_metas,
            -default=>$sort_metas->[0],
            -labels =>$name_labels
        ),
        $q->checkbox(
            -name   => 'reverse',
            -label  => 'Reverse Sort'
        );
}



sub get_index_select_list {
    my ( $results ) = @_;
    my $q = $results->CGI;


    my $indexes = $results->config('swish_index');
    return '' unless ref $indexes eq 'ARRAY';

    my $select_config = $results->config('select_indexes');
    return '' unless $select_config && ref $select_config eq 'HASH';


    # Should return a warning, as this might be a likely mistake
    # This jumps through hoops so that real index file name is not exposed
    
    return '' unless exists $select_config->{labels}
              && ref $select_config->{labels} eq 'ARRAY'
              && @$indexes == @{$select_config->{labels}};


    my @labels = @{$select_config->{labels}};
    my %map;

    for ( 0..$#labels ) {
        $map{$_} = $labels[$_];
    }

    my $method = $select_config->{method} || 'checkbox_group';
    my @cols = $select_config->{columns} ? ('-columns', $select_config->{columns}) : ();

    return join "\n",
        '<br>',
        ( $select_config->{description} || 'Select: '),
        $q->$method(
        -name   => 'si',
        -values => [0..$#labels],
        -default=> 0,
        -labels => \%map,
        @cols );
}


sub get_limit_select {
    my ( $results ) = @_;
    my $q = $results->CGI;


    my $limit = $results->config('select_by_meta');
    return '' unless ref $limit eq 'HASH';

    my $method = $limit->{method} || 'checkbox_group';

    my @options = (
        -name   => 'sbm',
        -values => $limit->{values},
        -labels => $limit->{labels} || {},
    );

    push @options, ( -columns=> $limit->{columns} ) if $limit->{columns};
    

    return join "\n",
        '<br>',
        ( $limit->{description} || 'Select: '),
        $q->$method( @options );
}
1;

