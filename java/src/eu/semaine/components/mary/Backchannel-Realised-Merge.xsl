<?xml version="1.0" encoding="iso-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
xmlns:mary="http://mary.dfki.de/2002/MaryXML"
xmlns:bml="http://www.mindmakers.org/projects/BML"
xmlns:ssml="http://www.w3.org/2001/10/synthesis"
exclude-result-prefixes="bml mary #default"
>

<xsl:output method="xml" encoding="ISO-8859-1" indent="yes"/>
<xsl:strip-space elements="*|text()"/>

<!-- load the merge file -->
<xsl:variable name="emps"
select="document('semaine.mary.realised.acoustics')"/>


  <xsl:template match="@*|node()">
    <xsl:copy>
       <xsl:apply-templates select="@*|node()"/>
    </xsl:copy>
  </xsl:template>


  <xsl:template match="bml:backchannel">

    <speech xmlns:ssml="http://www.w3.org/2001/10/synthesis" id="s1" language="en_US" text="">
	<ssml:mark name="tm1"/>
	<mary:syllable xmlns:mary="http://mary.dfki.de/2002/MaryXML" accent="1" stress="1">
        <xsl:apply-templates select="$emps/descendant::mary:nvv/*"/>
	</mary:syllable>
    	<ssml:mark name="tm2"/>
    </speech>	
  </xsl:template> 


  <xsl:template name="getToken" match="mary:ph">
     <xsl:variable name="duration" select="@d div 1000"/>
     <xsl:variable name="endTime" select="@end div 1000"/>
     <xsl:variable name="phone"  select="@p"/>
     <mary:ph d="{$duration}" end="{$endTime}" p="{$phone}"> </mary:ph>
   </xsl:template>

  
</xsl:stylesheet>

