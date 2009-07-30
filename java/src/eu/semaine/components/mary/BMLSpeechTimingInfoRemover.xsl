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
    <speech>	
        <xsl:copy-of select="@id"/>  
		<xsl:copy-of select="@type"/> 
		<xsl:copy-of select="@voice"/> 
		<xsl:copy-of select="@language"/>
        <xsl:copy-of select="@text"/> 
		<xsl:copy-of select="@speed_sym"/> 
        <xsl:apply-templates/>
    </speech>
  </xsl:template>

  <xsl:template match="mary:*">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template name="removeText" match="mary:*/text()"/>

</xsl:stylesheet>

