<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.12.0">
  <compound kind="file">
    <name>Environment.hpp</name>
    <path>rds2cpp/</path>
    <filename>Environment_8hpp.html</filename>
    <includes id="StringEncoding_8hpp" name="StringEncoding.hpp" local="yes" import="no" module="no" objc="no">StringEncoding.hpp</includes>
    <includes id="SEXPType_8hpp" name="SEXPType.hpp" local="yes" import="no" module="no" objc="no">SEXPType.hpp</includes>
    <class kind="struct">rds2cpp::Environment</class>
    <namespace>rds2cpp</namespace>
  </compound>
  <compound kind="file">
    <name>parse_rds.hpp</name>
    <path>rds2cpp/</path>
    <filename>parse__rds_8hpp.html</filename>
    <includes id="RdsFile_8hpp" name="RdsFile.hpp" local="yes" import="no" module="no" objc="no">RdsFile.hpp</includes>
    <class kind="struct">rds2cpp::ParseRdsOptions</class>
    <namespace>rds2cpp</namespace>
  </compound>
  <compound kind="file">
    <name>rds2cpp.hpp</name>
    <path>rds2cpp/</path>
    <filename>rds2cpp_8hpp.html</filename>
    <includes id="parse__rds_8hpp" name="parse_rds.hpp" local="yes" import="no" module="no" objc="no">parse_rds.hpp</includes>
    <includes id="write__rds_8hpp" name="write_rds.hpp" local="yes" import="no" module="no" objc="no">write_rds.hpp</includes>
    <namespace>rds2cpp</namespace>
  </compound>
  <compound kind="file">
    <name>RdsFile.hpp</name>
    <path>rds2cpp/</path>
    <filename>RdsFile_8hpp.html</filename>
    <includes id="RObject_8hpp" name="RObject.hpp" local="yes" import="no" module="no" objc="no">RObject.hpp</includes>
    <includes id="Environment_8hpp" name="Environment.hpp" local="yes" import="no" module="no" objc="no">Environment.hpp</includes>
    <includes id="Symbol_8hpp" name="Symbol.hpp" local="yes" import="no" module="no" objc="no">Symbol.hpp</includes>
    <class kind="struct">rds2cpp::RdsFile</class>
    <namespace>rds2cpp</namespace>
  </compound>
  <compound kind="file">
    <name>RObject.hpp</name>
    <path>rds2cpp/</path>
    <filename>RObject_8hpp.html</filename>
    <includes id="SEXPType_8hpp" name="SEXPType.hpp" local="yes" import="no" module="no" objc="no">SEXPType.hpp</includes>
    <includes id="StringEncoding_8hpp" name="StringEncoding.hpp" local="yes" import="no" module="no" objc="no">StringEncoding.hpp</includes>
    <class kind="struct">rds2cpp::RObject</class>
    <class kind="struct">rds2cpp::Null</class>
    <class kind="struct">rds2cpp::SymbolIndex</class>
    <class kind="struct">rds2cpp::EnvironmentIndex</class>
    <class kind="struct">rds2cpp::ExternalPointerIndex</class>
    <class kind="struct">rds2cpp::Attributes</class>
    <class kind="struct">rds2cpp::AtomicVector</class>
    <class kind="struct">rds2cpp::StringVector</class>
    <class kind="struct">rds2cpp::GenericVector</class>
    <class kind="struct">rds2cpp::PairList</class>
    <class kind="struct">rds2cpp::S4Object</class>
    <class kind="struct">rds2cpp::BuiltInFunction</class>
    <class kind="struct">rds2cpp::LanguageObject</class>
    <class kind="struct">rds2cpp::ExpressionVector</class>
    <namespace>rds2cpp</namespace>
  </compound>
  <compound kind="file">
    <name>SEXPType.hpp</name>
    <path>rds2cpp/</path>
    <filename>SEXPType_8hpp.html</filename>
    <namespace>rds2cpp</namespace>
  </compound>
  <compound kind="file">
    <name>StringEncoding.hpp</name>
    <path>rds2cpp/</path>
    <filename>StringEncoding_8hpp.html</filename>
    <namespace>rds2cpp</namespace>
  </compound>
  <compound kind="file">
    <name>Symbol.hpp</name>
    <path>rds2cpp/</path>
    <filename>Symbol_8hpp.html</filename>
    <includes id="StringEncoding_8hpp" name="StringEncoding.hpp" local="yes" import="no" module="no" objc="no">StringEncoding.hpp</includes>
    <class kind="struct">rds2cpp::Symbol</class>
    <namespace>rds2cpp</namespace>
  </compound>
  <compound kind="file">
    <name>write_rds.hpp</name>
    <path>rds2cpp/</path>
    <filename>write__rds_8hpp.html</filename>
    <includes id="RdsFile_8hpp" name="RdsFile.hpp" local="yes" import="no" module="no" objc="no">RdsFile.hpp</includes>
    <includes id="RObject_8hpp" name="RObject.hpp" local="yes" import="no" module="no" objc="no">RObject.hpp</includes>
    <namespace>rds2cpp</namespace>
  </compound>
  <compound kind="struct">
    <name>rds2cpp::AtomicVector</name>
    <filename>structrds2cpp_1_1AtomicVector.html</filename>
    <templarg>typename ElementType</templarg>
    <templarg>SEXPType stype</templarg>
    <base>rds2cpp::RObject</base>
    <member kind="function">
      <type>SEXPType</type>
      <name>type</name>
      <anchorfile>structrds2cpp_1_1AtomicVector.html</anchorfile>
      <anchor>a708beabc7687eea3b6145cb09de5a32f</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; ElementType &gt;</type>
      <name>data</name>
      <anchorfile>structrds2cpp_1_1AtomicVector.html</anchorfile>
      <anchor>a87427d2862c982a09ac077ac6ad12eae</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Attributes</type>
      <name>attributes</name>
      <anchorfile>structrds2cpp_1_1AtomicVector.html</anchorfile>
      <anchor>a6b1271e24cd3e456275de132de7e11e9</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>rds2cpp::Attributes</name>
    <filename>structrds2cpp_1_1Attributes.html</filename>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>structrds2cpp_1_1Attributes.html</anchorfile>
      <anchor>a6109e48b389399a860f1d98487a2b018</anchor>
      <arglist>(std::string n, RObject *v, StringEncoding enc=StringEncoding::UTF8)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>structrds2cpp_1_1Attributes.html</anchorfile>
      <anchor>abe5b330809ce9763404901fd282845e3</anchor>
      <arglist>(std::string n, std::unique_ptr&lt; RObject &gt; v, StringEncoding enc=StringEncoding::UTF8)</arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; std::string &gt;</type>
      <name>names</name>
      <anchorfile>structrds2cpp_1_1Attributes.html</anchorfile>
      <anchor>a3dd274a97e415b2d14606843b63f6b18</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; StringEncoding &gt;</type>
      <name>encodings</name>
      <anchorfile>structrds2cpp_1_1Attributes.html</anchorfile>
      <anchor>ae2db31e6b60cfa8b94257154dd283dc2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; std::unique_ptr&lt; RObject &gt; &gt;</type>
      <name>values</name>
      <anchorfile>structrds2cpp_1_1Attributes.html</anchorfile>
      <anchor>a5defb973382d2827821c68367c58968c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>rds2cpp::BuiltInFunction</name>
    <filename>structrds2cpp_1_1BuiltInFunction.html</filename>
    <base>rds2cpp::RObject</base>
    <member kind="function">
      <type>SEXPType</type>
      <name>type</name>
      <anchorfile>structrds2cpp_1_1BuiltInFunction.html</anchorfile>
      <anchor>ae67e34eec0eb4d75fc8b8d44a8d165e4</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="variable">
      <type>std::string</type>
      <name>name</name>
      <anchorfile>structrds2cpp_1_1BuiltInFunction.html</anchorfile>
      <anchor>aed011178abe0ebee7d2ec13f1af9730e</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>rds2cpp::Environment</name>
    <filename>structrds2cpp_1_1Environment.html</filename>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>structrds2cpp_1_1Environment.html</anchorfile>
      <anchor>a7d0eb5c0cd41bf1552eba6f6575bf1b6</anchor>
      <arglist>(std::string n, RObject *v, StringEncoding enc=StringEncoding::UTF8)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>structrds2cpp_1_1Environment.html</anchorfile>
      <anchor>ab30ee06c3010fa741a015b0f4cb6ab21</anchor>
      <arglist>(std::string n, std::unique_ptr&lt; RObject &gt; v, StringEncoding enc=StringEncoding::UTF8)</arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>locked</name>
      <anchorfile>structrds2cpp_1_1Environment.html</anchorfile>
      <anchor>a76e06486a983edc119e0aacfdd6b3273</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>hashed</name>
      <anchorfile>structrds2cpp_1_1Environment.html</anchorfile>
      <anchor>a3df071e70e181e3a763f09fd5c13162b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>SEXPType</type>
      <name>parent_type</name>
      <anchorfile>structrds2cpp_1_1Environment.html</anchorfile>
      <anchor>af916025aa24b767482eeea48f8f32512</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>parent</name>
      <anchorfile>structrds2cpp_1_1Environment.html</anchorfile>
      <anchor>ae729557dda55babd8df86b1edd6bfa1e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; std::string &gt;</type>
      <name>variable_names</name>
      <anchorfile>structrds2cpp_1_1Environment.html</anchorfile>
      <anchor>a667f7a97b340ebaf3d5a24c33929e60c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; StringEncoding &gt;</type>
      <name>variable_encodings</name>
      <anchorfile>structrds2cpp_1_1Environment.html</anchorfile>
      <anchor>addd5af039f70a716bc6088700d94a414</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; std::unique_ptr&lt; RObject &gt; &gt;</type>
      <name>variable_values</name>
      <anchorfile>structrds2cpp_1_1Environment.html</anchorfile>
      <anchor>afdaff9bbd4bf0b2e8a505e219b41000e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Attributes</type>
      <name>attributes</name>
      <anchorfile>structrds2cpp_1_1Environment.html</anchorfile>
      <anchor>a0143e140f79fac307aa93dba58283fc8</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>rds2cpp::EnvironmentIndex</name>
    <filename>structrds2cpp_1_1EnvironmentIndex.html</filename>
    <base>rds2cpp::RObject</base>
    <member kind="function">
      <type></type>
      <name>EnvironmentIndex</name>
      <anchorfile>structrds2cpp_1_1EnvironmentIndex.html</anchorfile>
      <anchor>ac50ff843b19bfaa8d3395bea0a517a38</anchor>
      <arglist>(SEXPType e=SEXPType::GLOBALENV_)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EnvironmentIndex</name>
      <anchorfile>structrds2cpp_1_1EnvironmentIndex.html</anchorfile>
      <anchor>a968a0c890622b9e7514e8dd16674960b</anchor>
      <arglist>(size_t i)</arglist>
    </member>
    <member kind="function">
      <type>SEXPType</type>
      <name>type</name>
      <anchorfile>structrds2cpp_1_1EnvironmentIndex.html</anchorfile>
      <anchor>a919fd1864585a46ae8841a59726fc491</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>index</name>
      <anchorfile>structrds2cpp_1_1EnvironmentIndex.html</anchorfile>
      <anchor>a48847650f3aacbb87796282b9124492d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>SEXPType</type>
      <name>env_type</name>
      <anchorfile>structrds2cpp_1_1EnvironmentIndex.html</anchorfile>
      <anchor>a1bf1c15fce2af818ff9b6cce9e12ae8f</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>rds2cpp::ExpressionVector</name>
    <filename>structrds2cpp_1_1ExpressionVector.html</filename>
    <base>rds2cpp::RObject</base>
    <member kind="function">
      <type>SEXPType</type>
      <name>type</name>
      <anchorfile>structrds2cpp_1_1ExpressionVector.html</anchorfile>
      <anchor>ac289540e98afc883b5eabe4d1685fe07</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; std::unique_ptr&lt; RObject &gt; &gt;</type>
      <name>data</name>
      <anchorfile>structrds2cpp_1_1ExpressionVector.html</anchorfile>
      <anchor>aadc9b11e73169122c1a9e0a70de00044</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Attributes</type>
      <name>attributes</name>
      <anchorfile>structrds2cpp_1_1ExpressionVector.html</anchorfile>
      <anchor>aba55dae94f564921d4ef232be76ee2d6</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>rds2cpp::ExternalPointerIndex</name>
    <filename>structrds2cpp_1_1ExternalPointerIndex.html</filename>
    <base>rds2cpp::RObject</base>
    <member kind="function">
      <type></type>
      <name>ExternalPointerIndex</name>
      <anchorfile>structrds2cpp_1_1ExternalPointerIndex.html</anchorfile>
      <anchor>ab2354f8940040b0236a2cbfdf5f5a3e3</anchor>
      <arglist>(size_t i=-1)</arglist>
    </member>
    <member kind="function">
      <type>SEXPType</type>
      <name>type</name>
      <anchorfile>structrds2cpp_1_1ExternalPointerIndex.html</anchorfile>
      <anchor>abec95f85216adc4df0b13d6641271a39</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>index</name>
      <anchorfile>structrds2cpp_1_1ExternalPointerIndex.html</anchorfile>
      <anchor>acaea291afd1811bda8b37e7396dcecb5</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>rds2cpp::GenericVector</name>
    <filename>structrds2cpp_1_1GenericVector.html</filename>
    <base>rds2cpp::RObject</base>
    <member kind="function">
      <type>SEXPType</type>
      <name>type</name>
      <anchorfile>structrds2cpp_1_1GenericVector.html</anchorfile>
      <anchor>a43b5fa96e496b57a7090523146078353</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; std::unique_ptr&lt; RObject &gt; &gt;</type>
      <name>data</name>
      <anchorfile>structrds2cpp_1_1GenericVector.html</anchorfile>
      <anchor>aca411f5b3a5609e4ba6fb450b4f935cb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Attributes</type>
      <name>attributes</name>
      <anchorfile>structrds2cpp_1_1GenericVector.html</anchorfile>
      <anchor>ac72b1a04ce4ca685e1121b075f59d486</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>rds2cpp::LanguageObject</name>
    <filename>structrds2cpp_1_1LanguageObject.html</filename>
    <base>rds2cpp::RObject</base>
    <member kind="function">
      <type>SEXPType</type>
      <name>type</name>
      <anchorfile>structrds2cpp_1_1LanguageObject.html</anchorfile>
      <anchor>ae09e6c629273aa154bbb410871826878</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_argument</name>
      <anchorfile>structrds2cpp_1_1LanguageObject.html</anchorfile>
      <anchor>a6152a7f57b3694cbeeb5a14cc0a5daf1</anchor>
      <arglist>(std::string n, RObject *d, StringEncoding enc=StringEncoding::UTF8)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_argument</name>
      <anchorfile>structrds2cpp_1_1LanguageObject.html</anchorfile>
      <anchor>a73e2db19ead7661419f0c82e16e4385c</anchor>
      <arglist>(std::string n, std::unique_ptr&lt; RObject &gt; d, StringEncoding enc=StringEncoding::UTF8)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_argument</name>
      <anchorfile>structrds2cpp_1_1LanguageObject.html</anchorfile>
      <anchor>ade1aa37398cdc8bfb80142d56398c5bf</anchor>
      <arglist>(RObject *d)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_argument</name>
      <anchorfile>structrds2cpp_1_1LanguageObject.html</anchorfile>
      <anchor>ae278b8b8ba14dc782acdab8312f4310a</anchor>
      <arglist>(std::unique_ptr&lt; RObject &gt; d)</arglist>
    </member>
    <member kind="variable">
      <type>std::string</type>
      <name>function_name</name>
      <anchorfile>structrds2cpp_1_1LanguageObject.html</anchorfile>
      <anchor>ab70121438413651f1e6e6fc95a0bb7c0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>StringEncoding</type>
      <name>function_encoding</name>
      <anchorfile>structrds2cpp_1_1LanguageObject.html</anchorfile>
      <anchor>a88a7d031093931584fa21490500edc47</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; std::unique_ptr&lt; RObject &gt; &gt;</type>
      <name>argument_values</name>
      <anchorfile>structrds2cpp_1_1LanguageObject.html</anchorfile>
      <anchor>a93c6bc2447024ed6a6a4233e00d076b7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; std::string &gt;</type>
      <name>argument_names</name>
      <anchorfile>structrds2cpp_1_1LanguageObject.html</anchorfile>
      <anchor>a094c3c56d3762a20091b097b2b7b2c4f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; unsigned char &gt;</type>
      <name>argument_has_name</name>
      <anchorfile>structrds2cpp_1_1LanguageObject.html</anchorfile>
      <anchor>aee5435bac7ed40bb5eaeb78bb05e5835</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; StringEncoding &gt;</type>
      <name>argument_encodings</name>
      <anchorfile>structrds2cpp_1_1LanguageObject.html</anchorfile>
      <anchor>a300aa25dab930fb8dcc6c05c3cf4530c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Attributes</type>
      <name>attributes</name>
      <anchorfile>structrds2cpp_1_1LanguageObject.html</anchorfile>
      <anchor>aae813b5cc43ee97f594b9c96a5c70cd0</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>rds2cpp::Null</name>
    <filename>structrds2cpp_1_1Null.html</filename>
    <base>rds2cpp::RObject</base>
    <member kind="function">
      <type>SEXPType</type>
      <name>type</name>
      <anchorfile>structrds2cpp_1_1Null.html</anchorfile>
      <anchor>afbb49b7f52faa2f0f460b7a99bc5cb52</anchor>
      <arglist>() const</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>rds2cpp::PairList</name>
    <filename>structrds2cpp_1_1PairList.html</filename>
    <base>rds2cpp::RObject</base>
    <member kind="function">
      <type>SEXPType</type>
      <name>type</name>
      <anchorfile>structrds2cpp_1_1PairList.html</anchorfile>
      <anchor>a66acd5fb763645bdbd5880e82c5d3805</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>structrds2cpp_1_1PairList.html</anchorfile>
      <anchor>a8e07c228c50a439fcf4d4747cf100b4b</anchor>
      <arglist>(std::string t, std::unique_ptr&lt; RObject &gt; d, StringEncoding enc=StringEncoding::UTF8)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>structrds2cpp_1_1PairList.html</anchorfile>
      <anchor>aa0f20e75f3eedeae77c240defd7fcd25</anchor>
      <arglist>(std::string t, RObject *d, StringEncoding enc=StringEncoding::UTF8)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>structrds2cpp_1_1PairList.html</anchorfile>
      <anchor>a84ac16335499a2f56636407ff3874c94</anchor>
      <arglist>(std::unique_ptr&lt; RObject &gt; d)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>structrds2cpp_1_1PairList.html</anchorfile>
      <anchor>a3a5bda24efb1fe1dbd15b103dfeda3a8</anchor>
      <arglist>(RObject *d)</arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; std::unique_ptr&lt; RObject &gt; &gt;</type>
      <name>data</name>
      <anchorfile>structrds2cpp_1_1PairList.html</anchorfile>
      <anchor>a1bff4d9a02cd64c44908b9362317a893</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; unsigned char &gt;</type>
      <name>has_tag</name>
      <anchorfile>structrds2cpp_1_1PairList.html</anchorfile>
      <anchor>ab5db0bbafb2affaa0f64532cb4c65388</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; std::string &gt;</type>
      <name>tag_names</name>
      <anchorfile>structrds2cpp_1_1PairList.html</anchorfile>
      <anchor>a36dc7df750a2aaaccfc943a1e3a3075a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; StringEncoding &gt;</type>
      <name>tag_encodings</name>
      <anchorfile>structrds2cpp_1_1PairList.html</anchorfile>
      <anchor>a815b78782ffa0f898b96df8fcaaccd0b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Attributes</type>
      <name>attributes</name>
      <anchorfile>structrds2cpp_1_1PairList.html</anchorfile>
      <anchor>a1ad992e6b3c70f83421e21fbf5ddfd71</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>rds2cpp::ParseRdsOptions</name>
    <filename>structrds2cpp_1_1ParseRdsOptions.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>parallel</name>
      <anchorfile>structrds2cpp_1_1ParseRdsOptions.html</anchorfile>
      <anchor>a1dd6bd4c10e4685d02cc45f6c60fc6f2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>byteme::SomeFileReaderOptions</type>
      <name>file_options</name>
      <anchorfile>structrds2cpp_1_1ParseRdsOptions.html</anchorfile>
      <anchor>ace40bcfb11a4966058d06c5a988202ce</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>rds2cpp::RdsFile</name>
    <filename>structrds2cpp_1_1RdsFile.html</filename>
    <member kind="variable">
      <type>uint32_t</type>
      <name>format_version</name>
      <anchorfile>structrds2cpp_1_1RdsFile.html</anchorfile>
      <anchor>ab6c650f54af12a20cf39bb2f677411db</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::array&lt; unsigned char, 3 &gt;</type>
      <name>writer_version</name>
      <anchorfile>structrds2cpp_1_1RdsFile.html</anchorfile>
      <anchor>a24c37c4e7b3356ed5a1301dec32d4b78</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::array&lt; unsigned char, 3 &gt;</type>
      <name>reader_version</name>
      <anchorfile>structrds2cpp_1_1RdsFile.html</anchorfile>
      <anchor>af2f3476dad325933efd2ae685bb0491b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::string</type>
      <name>encoding</name>
      <anchorfile>structrds2cpp_1_1RdsFile.html</anchorfile>
      <anchor>aa8a14e6d1edc325b3316576f75ddc4fb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::unique_ptr&lt; RObject &gt;</type>
      <name>object</name>
      <anchorfile>structrds2cpp_1_1RdsFile.html</anchorfile>
      <anchor>a4d451375c25d57d203169dce2bed1507</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; Environment &gt;</type>
      <name>environments</name>
      <anchorfile>structrds2cpp_1_1RdsFile.html</anchorfile>
      <anchor>aebd7594b4d63ea1537a6c460b482ce00</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; Symbol &gt;</type>
      <name>symbols</name>
      <anchorfile>structrds2cpp_1_1RdsFile.html</anchorfile>
      <anchor>a5ed3f4f1d927bbba3b9803d77f440968</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; ExternalPointer &gt;</type>
      <name>external_pointers</name>
      <anchorfile>structrds2cpp_1_1RdsFile.html</anchorfile>
      <anchor>a643c9a2ecb62557a061c52babc542b80</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>rds2cpp::RObject</name>
    <filename>structrds2cpp_1_1RObject.html</filename>
    <member kind="function" virtualness="pure">
      <type>virtual SEXPType</type>
      <name>type</name>
      <anchorfile>structrds2cpp_1_1RObject.html</anchorfile>
      <anchor>a650039d33738c4d5b91d0df0977f6d9b</anchor>
      <arglist>() const =0</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>rds2cpp::S4Object</name>
    <filename>structrds2cpp_1_1S4Object.html</filename>
    <base>rds2cpp::RObject</base>
    <member kind="function">
      <type>SEXPType</type>
      <name>type</name>
      <anchorfile>structrds2cpp_1_1S4Object.html</anchorfile>
      <anchor>adcf2f0a6a6fbe9c647e13c45c1f8505e</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="variable">
      <type>std::string</type>
      <name>class_name</name>
      <anchorfile>structrds2cpp_1_1S4Object.html</anchorfile>
      <anchor>a7cb839f83694dc70d66c529848fd1e5f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>StringEncoding</type>
      <name>class_encoding</name>
      <anchorfile>structrds2cpp_1_1S4Object.html</anchorfile>
      <anchor>a9aaea2ba7196414bc1f2118303de6887</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::string</type>
      <name>package_name</name>
      <anchorfile>structrds2cpp_1_1S4Object.html</anchorfile>
      <anchor>ac01786753fa1f33a5382527df8c42450</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>StringEncoding</type>
      <name>package_encoding</name>
      <anchorfile>structrds2cpp_1_1S4Object.html</anchorfile>
      <anchor>a048e7b648052c8b12d7fc5b2bcca38c9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Attributes</type>
      <name>attributes</name>
      <anchorfile>structrds2cpp_1_1S4Object.html</anchorfile>
      <anchor>afff39c9a8ab91c50d8542f0b583982f8</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>rds2cpp::StringVector</name>
    <filename>structrds2cpp_1_1StringVector.html</filename>
    <base>rds2cpp::RObject</base>
    <member kind="function">
      <type>SEXPType</type>
      <name>type</name>
      <anchorfile>structrds2cpp_1_1StringVector.html</anchorfile>
      <anchor>a8bfa8ead6efbb3e7fbe58580c40a1c61</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>structrds2cpp_1_1StringVector.html</anchorfile>
      <anchor>a2ae3a82394b91bef285ad744f537fc39</anchor>
      <arglist>(std::string d, StringEncoding enc=StringEncoding::UTF8)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>structrds2cpp_1_1StringVector.html</anchorfile>
      <anchor>af1c3c7c50cf073f30255a52460133e42</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; std::string &gt;</type>
      <name>data</name>
      <anchorfile>structrds2cpp_1_1StringVector.html</anchorfile>
      <anchor>a800012da9c834c3b7aabbb8dd009e59b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; StringEncoding &gt;</type>
      <name>encodings</name>
      <anchorfile>structrds2cpp_1_1StringVector.html</anchorfile>
      <anchor>a3e0ee119cdb7013bfe956df6d66165d6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; char &gt;</type>
      <name>missing</name>
      <anchorfile>structrds2cpp_1_1StringVector.html</anchorfile>
      <anchor>a75ffb98789db717ebb9a3715fcac46df</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Attributes</type>
      <name>attributes</name>
      <anchorfile>structrds2cpp_1_1StringVector.html</anchorfile>
      <anchor>afb09e475b2c0a283c06f295626bc9410</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>rds2cpp::Symbol</name>
    <filename>structrds2cpp_1_1Symbol.html</filename>
    <member kind="variable">
      <type>std::string</type>
      <name>name</name>
      <anchorfile>structrds2cpp_1_1Symbol.html</anchorfile>
      <anchor>a68099267ab64bc22a9aabbdd3d9fb87e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>StringEncoding</type>
      <name>encoding</name>
      <anchorfile>structrds2cpp_1_1Symbol.html</anchorfile>
      <anchor>a98cc68e484cb112a40021d4cb1687054</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>rds2cpp::SymbolIndex</name>
    <filename>structrds2cpp_1_1SymbolIndex.html</filename>
    <base>rds2cpp::RObject</base>
    <member kind="function">
      <type></type>
      <name>SymbolIndex</name>
      <anchorfile>structrds2cpp_1_1SymbolIndex.html</anchorfile>
      <anchor>a869b3accf667ee21d6e055ad475dc54c</anchor>
      <arglist>(size_t i=-1)</arglist>
    </member>
    <member kind="function">
      <type>SEXPType</type>
      <name>type</name>
      <anchorfile>structrds2cpp_1_1SymbolIndex.html</anchorfile>
      <anchor>a94aeb9118804d619fbf1a7b8269c9609</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>index</name>
      <anchorfile>structrds2cpp_1_1SymbolIndex.html</anchorfile>
      <anchor>ac28d8149a99c22f704505c81f1c9be2a</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>rds2cpp</name>
    <filename>namespacerds2cpp.html</filename>
    <class kind="struct">rds2cpp::AtomicVector</class>
    <class kind="struct">rds2cpp::Attributes</class>
    <class kind="struct">rds2cpp::BuiltInFunction</class>
    <class kind="struct">rds2cpp::Environment</class>
    <class kind="struct">rds2cpp::EnvironmentIndex</class>
    <class kind="struct">rds2cpp::ExpressionVector</class>
    <class kind="struct">rds2cpp::ExternalPointerIndex</class>
    <class kind="struct">rds2cpp::GenericVector</class>
    <class kind="struct">rds2cpp::LanguageObject</class>
    <class kind="struct">rds2cpp::Null</class>
    <class kind="struct">rds2cpp::PairList</class>
    <class kind="struct">rds2cpp::ParseRdsOptions</class>
    <class kind="struct">rds2cpp::RdsFile</class>
    <class kind="struct">rds2cpp::RObject</class>
    <class kind="struct">rds2cpp::S4Object</class>
    <class kind="struct">rds2cpp::StringVector</class>
    <class kind="struct">rds2cpp::Symbol</class>
    <class kind="struct">rds2cpp::SymbolIndex</class>
    <member kind="typedef">
      <type>AtomicVector&lt; int32_t, SEXPType::INT &gt;</type>
      <name>IntegerVector</name>
      <anchorfile>namespacerds2cpp.html</anchorfile>
      <anchor>a648cbbcf5f0bb0dae27d33af67ccf004</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>AtomicVector&lt; int32_t, SEXPType::LGL &gt;</type>
      <name>LogicalVector</name>
      <anchorfile>namespacerds2cpp.html</anchorfile>
      <anchor>a21605c7006141fdd9e46a065fafb2fb9</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>AtomicVector&lt; double, SEXPType::REAL &gt;</type>
      <name>DoubleVector</name>
      <anchorfile>namespacerds2cpp.html</anchorfile>
      <anchor>a3a233aabb04ace22331d96dd485292f4</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>AtomicVector&lt; unsigned char, SEXPType::RAW &gt;</type>
      <name>RawVector</name>
      <anchorfile>namespacerds2cpp.html</anchorfile>
      <anchor>a0adba04a729035c4b716a3d4db7cd340</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>AtomicVector&lt; std::complex&lt; double &gt;, SEXPType::CPLX &gt;</type>
      <name>ComplexVector</name>
      <anchorfile>namespacerds2cpp.html</anchorfile>
      <anchor>ad178dfcbc2a0ba9bd813f289bdf147a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>RdsFile</type>
      <name>Parsed</name>
      <anchorfile>namespacerds2cpp.html</anchorfile>
      <anchor>a9a4371900eec82463a3aa0a2a9d5cce1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>StringEncoding</name>
      <anchorfile>namespacerds2cpp.html</anchorfile>
      <anchor>a3af792550d3dc4537cdbd71452ef765b</anchor>
      <arglist></arglist>
      <enumvalue file="namespacerds2cpp.html" anchor="a3af792550d3dc4537cdbd71452ef765bab50339a10e1de285ac99d4c3990b8693">NONE</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="a3af792550d3dc4537cdbd71452ef765ba211978e3c996960decd60e0cbfe7184e">LATIN1</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="a3af792550d3dc4537cdbd71452ef765baeb7ee0fb585e2ac64fdc086466c474b1">UTF8</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="a3af792550d3dc4537cdbd71452ef765bad2cd8253361a9c732d21ca1d336599cc">ASCII</enumvalue>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>SEXPType</name>
      <anchorfile>namespacerds2cpp.html</anchorfile>
      <anchor>acf2b27eb21235c6dd30b8819ea682629</anchor>
      <arglist></arglist>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629abc0b1e8b63f3f00318eee5aeabc14255">NIL</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629af61acac3c77114ed6d54a7cf519208db">SYM</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629a298cb25408234de02baf2085803a464a">LIST</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629a08ca5075706229f7b91851d3631fb552">CLO</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629a84b2faa3b60428ae499f9c6672c1123d">ENV</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629ac689ad2fe350c82852e0d5882530bd15">PROM</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629a8559c2e57b465f0f820618ee4239cfe6">LANG</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629aa72e4a34f10af98a9814d865e717399f">SPECIAL</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629afbb600bf3200d9d665bf61bb07f29472">BUILTIN</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629a027acd3aa6b1dd7f26119d3cf0f9a063">CHAR</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629a61bb91d0869c65442f7566c62b5675a6">LGL</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629a53f93baa3057821107c750323892fa92">INT</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629a8cf125b0e31559ba75a9d9b4f818a554">REAL</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629a958448d4909b92010c81d52d3e6ad8f1">CPLX</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629a3fe0dfff438296bb525e0e8642586c2d">STR</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629a40679521b5da0954b705341a2859f782">DOT</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629a8e1bde3c3d303163521522cf1d62f21f">ANY</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629a390853dfff13724af5495d195be51bb2">VEC</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629ab0cd8af157ed30eaa1980da18b07a38d">EXPR</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629af33c5d819668b70b0407e53398711438">BCODE</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629a8e19678be70c42cbf68067d2bfeface9">EXTPTR</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629a5772005a85d9f1121ecdda389403d6af">WEAKREF</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629a633d6abff0a3fc22404347728d195059">RAW</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629ad59a89abb4af5bc4dd85243d5f75bcac">S4</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629a727e346d3e645e255bb1d17bde1e3256">REF</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629a56c2def1ce91b8632e998c76bbd866d6">NILVALUE_</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629a4fac6441f852cda894d2dba022648519">GLOBALENV_</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629a57c96154e76e82d165e2583107d63b21">UNBOUNDVALUE_</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629ac2f03a612492417d254252fe94a8d2ed">MISSINGARG_</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629a71c5d6e400968b60b3b7c23c6d32fd1c">BASENAMESPACE_</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629a7347fe5a0f184f79ef064e92e3beb297">NAMESPACE</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629ae83af69d4844921d55507863e9099eb2">PACKAGE</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629a78cf276225eedfa810e4d1bf4d9fb17c">PERSIST</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629a231a309c9cb19a59d5452efde044c1a8">CLASSREF</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629a41fd7da53275bbbbb959476544f5738a">GENERICREF</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629a94805426b79f166bc5fcbee80146b6b2">BCREPDEF</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629a391c38cb75f9fceb63f2ed47397c0a2b">BCREPREF</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629ac8a3c0eb75ed523dade266b471e1a855">EMPTYENV_</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629a81d3c049d846aa662548ac2abf57295e">BASEENV_</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629ab32bdae68f7552c239b4424f39cfa2c0">ATTRLANG</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629aa22eb73117cf8a97c0bb0bda2ce20bf6">ATTRLIST</enumvalue>
      <enumvalue file="namespacerds2cpp.html" anchor="acf2b27eb21235c6dd30b8819ea682629aaee609eea0155bbd24d2569f0af1c037">ALTREP_</enumvalue>
    </member>
    <member kind="function">
      <type>RdsFile</type>
      <name>parse_rds</name>
      <anchorfile>namespacerds2cpp.html</anchorfile>
      <anchor>a6e8135078d12460c602af9c3933eb435</anchor>
      <arglist>(Reader_ &amp;reader, const ParseRdsOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>RdsFile</type>
      <name>parse_rds</name>
      <anchorfile>namespacerds2cpp.html</anchorfile>
      <anchor>a241c3b1eaf4075a72418c89407e76a20</anchor>
      <arglist>(std::string file, const ParseRdsOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write_rds</name>
      <anchorfile>namespacerds2cpp.html</anchorfile>
      <anchor>a5c53787e1d5fea7c789ba4fd4fab0849</anchor>
      <arglist>(const RdsFile &amp;info, Writer &amp;writer)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write_rds</name>
      <anchorfile>namespacerds2cpp.html</anchorfile>
      <anchor>ae3c23d14258573a432b6cdfeba665108</anchor>
      <arglist>(const RdsFile &amp;info, const char *path)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write_rds</name>
      <anchorfile>namespacerds2cpp.html</anchorfile>
      <anchor>a93b6a68b81f3697000b504c6d8950f43</anchor>
      <arglist>(const RdsFile &amp;info, std::string path)</arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Read RDS files in C++</title>
    <filename>index.html</filename>
    <docanchor file="index.html">md__2github_2workspace_2README</docanchor>
  </compound>
</tagfile>
