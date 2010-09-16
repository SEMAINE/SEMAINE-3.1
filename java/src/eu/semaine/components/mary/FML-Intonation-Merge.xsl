<?xml version="1.0" encoding="iso-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
xmlns:mary="http://mary.dfki.de/2002/MaryXML"
xmlns:bml="http://www.mindmakers.org/projects/BML"
xmlns="http://www.mindmakers.org/projects/BML"
xmlns:ssml="http://www.w3.org/2001/10/synthesis"
exclude-result-prefixes="bml mary #default"
>

<xsl:output method="xml" encoding="ISO-8859-1" indent="yes"/>
<xsl:strip-space elements="*|text()"/>

  <xsl:template match="@*|node()">
    <xsl:copy>
       <xsl:apply-templates select="@*|node()"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="bml:speech">
    <bml:speech>	
        <xsl:copy-of select="@id"/>  
	<xsl:copy-of select="@type"/> 
	<xsl:copy-of select="@voice"/> 
	<xsl:copy-of select="@language"/>
        <xsl:copy-of select="@text"/> 
	<xsl:copy-of select="@speed_sym"/> 
        <xsl:apply-templates/>
        <xsl:apply-templates select="document('semaine.mary.intonation')"/>
    </bml:speech>
  </xsl:template>

  <xsl:template match="mary:*">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template name="getBoundary"  match="mary:boundary">
       
      
     <xsl:variable name="nextTimeMark">  
      	<xsl:value-of select="following::mary:mark[1]/@name"/>         
      </xsl:variable>
      
      <xsl:variable name="countData" select="position()"/>
      <boundary id="b{$countData}" time="{$nextTimeMark}"/>
       
   </xsl:template>


    
   <xsl:template name="getPitchAccent" match="mary:t">
      <xsl:if test="@accent">
      <xsl:variable name="nextTimeMark">  
      	<xsl:value-of select="following::mary:mark[1]/@name"/>         
      </xsl:variable>
      <xsl:variable name="prevTimeMark">  
      	<xsl:value-of select="preceding::mary:mark[1]/@name"/>         
      </xsl:variable>
      <xsl:variable name="index">  
      	<xsl:value-of select="position()"/>         
      </xsl:variable>
      <pitchaccent id="xpa{$index}" start="{$prevTimeMark}" end="{$nextTimeMark}"/>  
      </xsl:if>
   </xsl:template>


   <xsl:template name="removeText" match="mary:*/text()"/>

</xsl:stylesheet>

