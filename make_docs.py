#!/usr/bin/env python
# -*- coding: utf-8 -*-

import re
import os
import hashlib


RE_COMMENT = re.compile('(/\*\*\s*(.*?)\s*\*/)', re.U|re.S)
RE_FUNC_DESCR = re.compile('([^\n]+)\n(.*)', re.U|re.S)
RE_FUNC_PARAMS = re.compile('(?P<type>\S+)\s+(?P<name>\S+)', re.U)

SOURCE_FILES = ['dwavd.c']


def parse_files():
    """Parses source file, extracting lines in /** */ comments
    and form a ``docstrings`` dictionary.

    Sections are deduced from ``/** ======= Basic ======= */`` comments.

    Comment example::

        /** mixed dwavd_init(string host, int port, string login, string password[, string certificate])

           Initializes a connection resource.
           Returns initialized resource on success, or NULL otherwise. */

    """

    docstrings = {'': []}

    for filepath in SOURCE_FILES:
        contents = read_from_file(filepath)

        docstrings_in_file = {'': []}

        current_chapter = ''

        for match in re.finditer(RE_COMMENT, contents):
            comment = match.group(2)
            match = re.match(RE_FUNC_DESCR, comment)
            if match is None:
                # Verify whether it is a chapter title
                if comment.startswith('=='):
                    chapter_title = comment.strip('=').strip()
                    if chapter_title:
                        current_chapter = chapter_title
                        if current_chapter not in docstrings_in_file:
                            docstrings_in_file[current_chapter] = []
                else:
                    print 'WARNING: no description in "%s"' % comment
            else:
                fdef, fdescr = match.groups()
                docstrings_in_file[current_chapter].append((fdef, fdescr.strip('\n').strip()))

        docstrings.update(docstrings_in_file)

    if not len(docstrings['']):
        del docstrings['']

    return docstrings


def generate_html(docstrings):
    """Generates and returns html document based on a given ``docstrings`` dictionary."""

    page_title = 'Dr.Web php-avdesk module API documentation'

    body = []
    toc = []

    # Arrange chapters in alphabetical order.
    for chapter in sorted(docstrings.iterkeys()):
        strings = docstrings[chapter]

        if not chapter:
            chapter = 'Common'
        chapter_hash = hashlib.md5(chapter).hexdigest()

        body.append('<a name="%s"></a><h3>%s</h3>' % (chapter_hash, chapter))

        toc_subs = []
        for fdef, fdescr in strings:
            fdef = fdef.strip()
            fdef_hash = hashlib.md5('%s%s' % (chapter, fdef)).hexdigest()
            if fdef.endswith(')'):
                fdef_stripped = fdef.split(' ', 1)[1].split('(', 1)[0]
                f_rettype, f_sign = fdef.split(' ', 1)
                f_name, f_params = f_sign.split('(', 1)
                f_params = re.findall(RE_FUNC_PARAMS, f_params[:-1])
                f_params = ''.join(['<span class="f_rettype">%s</span> <span class="f_parname">%s</span> ' % (ptype, pname) for ptype, pname in f_params]).strip()
                fdef = '<span class="f_rettype">%s</span> <span class="f_name">%s</span> (<span class="f_params">%s</span>)' % (f_rettype, f_name, f_params)
            else:
                fdef_stripped = fdef

            fdescr_ = []
            for line in fdescr.splitlines():
                if ' -' in line:
                    subj, descr = line.split(' -', 1)
                    fdescr_.append('<span class="f_parname">%s</span> <span class="p_descr">&#151; %s</span>' % (subj, descr))
                else:
                    fdescr_.append(line)
            fdescr = fdescr_
            del fdescr_

            toc_subs.append('<li><a href="#%s">%s</a></li>' % (fdef_hash, fdef_stripped))
            body.append('<div class="docstr_block"><a name="%s"></a><h4>%s</h4><div class="f_descr">%s</div><a href="#toc">^</a></div>' % (fdef_hash, fdef, '<br />'.join(fdescr)))

        toc.append('<li><a href="#%s">%s</a><ul>%s</ul></li>' % (chapter_hash, chapter, ''.join(toc_subs)))

    html_source = '''
<!doctype html>
<html>
<head>
    <title>{{ page_title }}</title>
    <meta charset="UTF-8">
    <style>
        body {
            font-size: 14px;
            color: #000;
        }
        hr {
            color: #eee;
        }
        ul {
            margin: 7px;
        }
        #readme {
            display: inline-block;
            padding: 10px;
            background-color: #f5f5f5;
            font-size: 13px;
            font-family: Courier;
            margin-bottom: 15px;
        }
        .f_rettype {
            color: #ccc;
        }
        .f_name {
            color: #000;
        }
        .f_params, .p_descr {
            color: #888;
        }
        .f_descr {
            margin-bottom: 25px;
        }
        .docstr_block {
            border-bottom: 1px solid #eee;
            padding-bottom: 10px;
            margin: 0 0 30px 40px;
        }
        .marg__b {
            margin-bottom: 20px;
        }
    </style>
</head>
<body>
    <h1>{{ page_title }}</h1>
    <div id="readme" class="marg__b">{{ readme_source }}</div>
    <div id="toc" class="marg__b">
        <a name="toc"></a>
        <h2>Table of contents</h2>
        {{ toc }}
    </div>
    <div id="doc">
        <hr noshade size="1" />
        <h2>API Documentation</h2>
        {{ body }}
    </div>
</body>
</html>
    '''
    html = html_source.replace('{{ page_title }}', page_title).\
        replace('{{ toc }}', '<ul>%s</ul>' % ''.join(toc)).\
        replace('{{ body }}', ''.join(body)).\
        replace('{{ readme_source }}', read_from_file('README').
        replace('\n', '<br />'))

    return html


def read_from_file(filepath):
    """Read strings from file."""
    f = open(filepath)
    contents = f.read()
    f.close()
    return contents


def write_to_file(target_file, contents):
    """Writes contents to file. Overwrites file if already exists."""
    f = open(target_file, 'w')
    f.write(contents)
    f.close()


if __name__=='__main__':

    print '==' * 30
    print 'This utility generates API documentation from source code.'
    print 'api_documentation.html file will be available in project root after run.\n'
    print '==' * 30

    docstrings = parse_files()
    html = generate_html(docstrings)
    write_to_file(os.path.abspath(os.path.join('.', 'api_documentation.html')), html)
