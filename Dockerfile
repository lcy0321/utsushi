#  Dockerfile -- to build a canonical development environment
#  Copyright © 2019-2021  Olaf Meeuwissen
#
#  License: GPL-3.0+
#  Author : Olaf Meeuwissen
#
#  This file is part of the 'Utsushi' package.
#  This package is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License or, at
#  your option, any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#  You ought to have received a copy of the GNU General Public License
#  along with this package.  If not, see <http://www.gnu.org/licenses/>.

ARG   codename=chimaera
FROM  registry.gitlab.com/paddy-hack/devuan/slim:$codename
LABEL maintainer="Olaf Meeuwissen <paddy-hack@member.fsf.org>" \
      projecturl="https://gitlab.com/utsushi/"

#  The tests/vc-dist-files file is generated using git but upstream's
#  ./install-deps assumes that that is already installed.  Make sure
#  that that's the case :-)

RUN apt-get --quiet update \
 && DEBIAN_FRONTEND=noninteractive \
    apt-get --quiet install --assume-yes \
            git

#  Use upstream's dependency installer to pull in most of the needed
#  packages.

COPY ./install-deps .

RUN apt-get --quiet update \
 && DEBIAN_FRONTEND=noninteractive \
    ./install-deps

#  Add a few extra packages for PDF documentation generation that are
#  not pulled in by ./install-deps unless pdflatex is present already
#  for some reason (probably size).

RUN apt-get --quiet update \
 && DEBIAN_FRONTEND=noninteractive \
    apt-get --quiet install --assume-yes \
            texlive-latex-base \
            texlive-fonts-recommended \
            texlive-latex-recommended \
            texlive-latex-extra
